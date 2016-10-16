#include "Document.h"
#include <arpa/inet.h>
#include "Editor.h"
#include "MainWindow.h"
#include <QFile>
#include <QPlainTextDocumentLayout>
#include <QThread>
#include "PreferenceDialog.h"
#include "RemoteSaveWorker.h"
#include "StatusBar.h"
#include "SyntaxHighlighter.h"

#define BUFFER_SIZE 1460

QHash<QString, QStringList> Document::extensions_ =
    QHash<QString, QStringList>();
QVector<Document*> Document::documents_ = QVector<Document*>();

Document::Document(const QString& path) :
    QTextDocument(),
    category_(tr("Default")),
    needToSave_(false),
    syntaxHighlighter_(new SyntaxHighlighter(this)),
    cursorPosition_(QHash<Editor*, int>()),
    scrollBarPosition_(QHash<Editor*, int>()),
    remotePath_(QString()),
    remoteSaveSuccessful_(true),
    thread_(new QThread),
    remoteSaveWorker_(new RemoteSaveWorker(this))
{
    setPath(path);
    setDocumentLayout(new QPlainTextDocumentLayout(this));
    determineName();
    determineCategory();
    setDefaultFont(Editor::editorFont());

    documents_.append(this);
    sortDocumentList();

    connect(this, SIGNAL(contentsChanged()), this, SLOT(modified()));

    remoteSaveWorker_->moveToThread(thread_);
    connect(thread_, SIGNAL(started()), remoteSaveWorker_, SLOT(process()));
    connect(remoteSaveWorker_, SIGNAL(finished()), thread_, SLOT(quit()));
}

Document::~Document()
{
    thread_->quit();
    delete thread_;
    delete remoteSaveWorker_;

    if (documents_.contains(this)) {
        documents_.remove(documents_.indexOf(this));
    }
}

bool
Document::open()
{
    bool result = false;
    QString filePath = path();
    QString fileName = name();
    QFile file(path());
    QByteArray byteArray;

    if (file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        byteArray = file.readAll();
        if (byteArray.size() > 0 &&
                byteArray.at(byteArray.size() - 1) == '\n') {
            byteArray.chop(1);
        }
        setPlainText(byteArray);
        file.close();

        needToSave_ = false;

        setPath(filePath);
        setName(fileName);

        result = true;
    }

    return result;
}

bool
Document::save()
{
    bool result = false;
    QFile file(path());

    if (file.open(QIODevice::Text  | QIODevice::WriteOnly |
            QIODevice::Truncate)) {
        file.write(toPlainText().toLatin1());
        file.write("\n", 1);
        file.close();

        result = true;
        needToSave_ = false;

        if (remotePath_.isEmpty() == false &&
                PreferenceDialog::get().serverAddress().isEmpty() == false &&
                PreferenceDialog::get().serverPassword().isEmpty() == false &&
                PreferenceDialog::get().serverPrivateKey().isEmpty() == false) {
            if (remotePath_.size() < 1024) {
                thread_->start();
            } else {
                PreferenceDialog::get().setRemoteErrorStatus(
                    PreferenceDialog::FilePathTooLong);
            }
        }
    }

    return result;
}

bool
Document::saveAs(const QString& path)
{
    bool result = false;
    QFile file(path);

    if (file.open(QIODevice::Text  | QIODevice::WriteOnly |
            QIODevice::Truncate)) {
        file.write(toPlainText().toLatin1());
        file.write("\n", 1);
        file.close();

        result = true;
        needToSave_ = false;

        setPath(path);
        determineName();

        sortDocumentList();
        MainWindow::get().updateDocumentsMenu();
        StatusBar::updateAllStatusBars();

        if (remotePath_.isEmpty() == false &&
                PreferenceDialog::get().serverAddress().isEmpty() == false &&
                PreferenceDialog::get().serverPassword().isEmpty() == false &&
                PreferenceDialog::get().serverPrivateKey().isEmpty() == false) {
            if (remotePath_.size() < 1024) {
                thread_->start();
            } else {
                PreferenceDialog::get().setRemoteErrorStatus(
                    PreferenceDialog::FilePathTooLong);
            }
        }
    }

    return result;
}

void
Document::rehighlightSyntax()
{
    syntaxHighlighter_->rehighlight();
}

void
Document::sortDocumentList()
{
    QVector<Document*> docs(documents_);
    documents_.clear();
    Document *doc;
    QString nameA;
    QString nameB;
    int nameIndexA;
    int nameIndexB;
    int namePos;
    int index;
    for (int docIndex = 0; docIndex < docs.size(); ++docIndex) {
        doc = docs.at(docIndex);

        nameA = doc->name();
        nameIndexA = -1;
        namePos = nameA.indexOf(" #");
        if (namePos >= 0) {
            nameIndexA = nameA.mid(namePos + 2).toInt();
            nameA = nameA.mid(0, namePos);
        }

        index = 0;
        while (index < documents_.size()) {
            nameB = documents_.at(index)->name();
            nameIndexB = -1;
            namePos = nameB.indexOf(" #");
            if (namePos >= 0) {
                nameIndexB = nameB.mid(namePos + 2).toInt();
                nameB = nameB.mid(0, namePos);
            }

            if (nameA < nameB || (nameA == nameB && nameIndexA < nameIndexB)) {
                break;
            }

            ++index;
        }

        documents_.insert(index, doc);
    }
}

void
Document::determineCategory()
{
    int index = path().lastIndexOf(".");
    if (index == -1) {
        category_ = "Default";
        return;
    }
    QString fileExtension = path().mid(index);

    QStringList categories(SyntaxHighlighter::categories());
    QString category = "Default";
    QStringList extensions;
    QString extension;
    QList<QString>::const_iterator iterA = categories.constBegin();
    QList<QString>::const_iterator iterB;
    while (iterA != categories.constEnd()) {
        category = *iterA;
        extensions = Document::extensions(category);
        iterB = extensions.constBegin();
        while (iterB != extensions.constEnd()) {
            extension = *iterB;
            if (extension == fileExtension) {
                category_ = category;
                return;
            }

            ++iterB;
        }

        ++iterA;
    }

    category_ = "Default";
}

int
Document::findMatchingName(const QString& name) const
{
    int result = -1;
    int index;
    bool intCheck;
    QVector<Document*>::const_iterator iter = documents_.constEnd();

    while (iter != documents_.constBegin()) {
        --iter;

        if (*iter != this) {
            if ((*iter)->name().indexOf(name) == 0) {
                if ((*iter)->name().size() > name.size() &&
                        (*iter)->name().at(name.size()) != ' ') {
                    continue;
                }

                index = (*iter)->name().indexOf(" #");
                if (index == name.size()) {
                    index = (*iter)->name().mid(index + 2).toInt(&intCheck);
                    if (intCheck == true && index > 1) {
                        result = index;
                        break;
                    }
                } else if (index == -1) {
                    result = 1;
                    break;
                }
            }
        }
    }

    return result;
}

void
Document::determineName()
{
    int index;
    QString name;

    if (path().isEmpty()) {
        name = "New File";
    } else {
        index = path().lastIndexOf("/");
        if (index == -1) {
            name = path();
        } else {
            name = path().mid(index + 1);
        }
    }

    index = findMatchingName(name);
    if (index != -1) {
        name = QString("%1 #%2").arg(name).arg(index + 1);
    }

    setName(name);
}

void
Document::modified()
{
    needToSave_ = true;
}

#ifndef INLINE
#include "Document.icc"
#endif // INLINE
