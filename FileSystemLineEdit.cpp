#include "FileSystemLineEdit.h"
#include <pwd.h>
#include <QAbstractItemView>
#include <QCompleter>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QValidator>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////

class Validator :
    public QValidator
{
public:
    Validator(QObject* parent = 0);

    virtual QValidator::State validate(QString& input, int& pos) const;
};

///////////////////////////////////////////////////////////////////////////////
// FileSystemLineEdit
///////////////////////////////////////////////////////////////////////////////

FileSystemLineEdit::FileSystemLineEdit(QWidget *parent) :
    QLineEdit(parent),
    completer_(new QCompleter)
{
    setCompleter(completer_);
    setValidator(new Validator(this));

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    model->setFilter(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot |
        QDir::CaseSensitive);
    completer_->setModel(model);
}

FileSystemLineEdit::~FileSystemLineEdit()
{
    delete validator();
    delete completer_->model();
    delete completer_;
}

QStringList
FileSystemLineEdit::paths() const
{
    QStringList result;
    QString editText = text();
    int index;
    QString currCompletion;
    QFileInfo fileInfo;

    if (editText.at(editText.size() - 1) == '*') {
        editText.remove(editText.size() - 1, 1);
        completer_->setCompletionPrefix(editText);
        index = 0;
        while (completer_->setCurrentRow(index)) {
            currCompletion = completer_->currentCompletion();
            fileInfo.setFile(currCompletion);
            if (fileInfo.isFile()) {
                result << currCompletion;
            }
            ++index;
        }
    } else {
        result << editText;
    }

    return result;
}

QString
FileSystemLineEdit::directory() const
{
    QString result(text());
    result.remove(result.lastIndexOf("/"), result.size());

    QDir dir(result);
    result = dir.absolutePath();

    return result;
}

bool
FileSystemLineEdit::event(QEvent* event)
{
    QKeyEvent *keyEvent;
    QString common(text());
    QString currCompletion;
    int index = 0;
    QFileInfo fileInfo;

    if (event->type() == QEvent::KeyPress) {
        keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent && keyEvent->key() == Qt::Key_Tab) {
            completer_->setCompletionPrefix(text());
            while (completer_->setCurrentRow(index)) {
                currCompletion = completer_->currentCompletion();
                if (index == 0) {
                    common = currCompletion;
                } else {
                    while (currCompletion.indexOf(common) == -1) {
                        common.remove(common.size() - 1, 1);
                    }
                }
                ++index;
            }
            fileInfo.setFile(common);
            if (fileInfo.isDir() && common.at(common.size() - 1) != '/') {
                common.append('/');
            }
            setText(common);
            completer_->complete();
            event->accept();
            return false;
        } else if (keyEvent && (keyEvent->key() == Qt::Key_Enter ||
                keyEvent->key() == Qt::Key_Return)) {
            completer_->popup()->hide();
            event->accept();
        } else if (keyEvent && keyEvent->key() == Qt::Key_Escape) {
            completer_->popup()->hide();
            event->accept();
        }
    } else if (event->type() == QEvent::FocusIn) {
        deselect();
        event->accept();
    }

    return QLineEdit::event(event);
}

///////////////////////////////////////////////////////////////////////////////
// Validator
///////////////////////////////////////////////////////////////////////////////

Validator::Validator(QObject* parent) :
    QValidator(parent)
{
}

QValidator::State
Validator::validate(QString& input, int& pos) const
{
    QValidator::State result = QValidator::Intermediate;
    int index;
    QString userName;
    QString fileName;
    QStringList entries;
    struct passwd *passWordData;
    QDir dir;
    QList<QString>::const_iterator iter;

    if (input.size() > 0) {
        if (input.at(0) == '~') {
            index = input.indexOf('/');
            if (index == -1) {
                // User probably has not finished typing the username.
                result = QValidator::Intermediate;
            } else if (index == 1) {
                // The home directory of the current user.
                userName = QString(getenv("USER"));
                passWordData = getpwnam(userName.toLatin1().data());
                if (passWordData) {
                    input = QString(passWordData->pw_dir) + "/";
                    pos = QString(passWordData->pw_dir).size() + 1;
                }
                result = QValidator::Acceptable;
            } else if (index > 1) {
                // A username is specified after the tilde.
                // Retrieve the username and determine the home directory if valid.
                // e.g. ~www-data/index.html
                userName = input.mid(1, index - 1);
                passWordData = getpwnam(userName.toLatin1().data());
                if (passWordData) {
                    input = QString(passWordData->pw_dir) + "/";
                    pos = QString(passWordData->pw_dir).size() + 1;
                    result = QValidator::Acceptable;
                } else {
                    result = QValidator::Invalid;
                }
            }
        } else {
            index = input.lastIndexOf('/');
            dir.setPath(input.mid(0, index));
            if (index < input.size() - 1) {
                fileName = input.mid(index + 1);
                if (fileName.endsWith('*')) {
                    fileName.remove(fileName.size() - 1, 1);
                }
                entries = dir.entryList(QDir::AllEntries | QDir::Hidden |
                    QDir::NoDotAndDotDot | QDir::CaseSensitive);
                iter = entries.constBegin();
                while (iter != entries.constEnd()) {
                    if (iter->indexOf(fileName) == 0) {
                        break;
                    }
                    ++iter;
                }
                if (iter == entries.constEnd()) {
                    result = QValidator::Invalid;
                } else {
                    result = QValidator::Acceptable;
                }
            } else {
                result = QValidator::Acceptable;
            }
        }
    }

    return result;
}
