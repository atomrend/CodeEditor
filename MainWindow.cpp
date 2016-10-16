#include "MainWindow.h"
#include "ActionEditor.h"
#include "Document.h"
#include "Editor.h"
#include "FileInfoDialog.h"
#include <math.h>
#include "PreferenceDialog.h"
#include "PreferenceFile.h"
#include <QAction>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QSplitter>
#include <QStatusBar>
#include <QSvgRenderer>
#include <QTimer>
#include "QuickOpenDialog.h"
#include "SaveListDialog.h"
#include "SearchReplaceDialog.h"
#include "StatusBar.h"
#include <stdlib.h>
#include "SyntaxHighlighter.h"

MainWindow* MainWindow::mainWindow_ = NULL;

MainWindow::MainWindow(
        const QStringList& files,
        QWidget* parent,
        Qt::WindowFlags f) :
    QMainWindow(parent, f),
    editorSplitter_(new QSplitter(Qt::Horizontal)),
    statusBar_(new QStatusBar),
    statusBarSplitter_(new QSplitter(Qt::Horizontal)),
    menuBar_(new QMenuBar),
    documentsMenu_(new QMenu("&Documents")),
    delayedFileOpenList_(files)
{
    // Delay the display of the GUI for 0.2 seconds.
    clock_t start = clock();
    while (clock() - start < CLOCKS_PER_SEC / 5);

    mainWindow_ = this;
    setWindowTitle("Codedit");
    setWindowState(Qt::WindowMaximized);

    QSvgRenderer svg(QString(":/icon.svg"));
    QRectF viewBox = svg.viewBoxF();
    float aspectRatio = viewBox.width() / viewBox.height();
    int width = 48;
    int height = 48;
    if (aspectRatio > 1) {
        // Wider.
        aspectRatio = 48 / aspectRatio;
        height = floor(aspectRatio + 0.5f);
    } else {
        // Taller.
        aspectRatio = 48 * aspectRatio;
        width = floor(aspectRatio + 0.5f);
    }
    QPixmap pixmap(width, height);
    pixmap.fill(QColor(Qt::transparent));
    QPainter painter(&pixmap);
    svg.render(&painter);
    painter.end();
    setWindowIcon(pixmap);

    setCentralWidget(editorSplitter_);
    createMenuBar();
    createFileMenu();
    createEditMenu();
    createOptionsMenu();
    createDocumentsMenu();
    show();
    setWindowState(Qt::WindowMaximized);

    QString homeDir(getenv("HOME"));
    homeDir += "/.config/codedit.xml";
    prefResult_ = readPreferenceFile(homeDir);
    QTimer::singleShot(10, this, SLOT(onDelayedSizing()));

    if (!delayedFileOpenList_.isEmpty()) {
        openFile(delayedFileOpenList_[0]);
        QTimer::singleShot(500, this, SLOT(onDelayedTimeout()));
    } else {
        newDocument();
    }

    setStatusBar(statusBar_);
    statusBar_->setSizeGripEnabled(false);
    statusBar_->addPermanentWidget(statusBarSplitter_, 1);
    connect(statusBarSplitter_, SIGNAL(splitterMoved(int, int)),
        this, SLOT(onStatusBarSplitterMoved(int, int)));
    connect(editorSplitter_, SIGNAL(splitterMoved(int, int)),
        this, SLOT(onEditorSplitterMoved(int, int)));

    createEditor();
}

MainWindow::~MainWindow()
{
}

bool
MainWindow::isMenuBarVisible() const
{
    return menuBar_->isVisible();
}

void
MainWindow::setMenuBarVisible(bool vis)
{
    menuBar_->setVisible(vis);
    menuBarAct_->setIcon(vis ? QIcon(":/check.png") : QIcon());
}

bool
MainWindow::isStatusBarVisible() const
{
    return statusBar_->isVisible();
}

void
MainWindow::setStatusBarVisible(bool vis)
{
    statusBar_->setVisible(vis);
    statusBarAct_->setIcon(vis ? QIcon(":/check.png") : QIcon());
}

void
MainWindow::updateDocumentsMenu()
{
    doc00Act_->setVisible(false);
    doc01Act_->setVisible(false);
    doc02Act_->setVisible(false);
    doc03Act_->setVisible(false);
    doc04Act_->setVisible(false);
    doc05Act_->setVisible(false);
    doc06Act_->setVisible(false);
    doc07Act_->setVisible(false);
    doc08Act_->setVisible(false);
    doc09Act_->setVisible(false);
    doc10Act_->setVisible(false);
    doc11Act_->setVisible(false);
    doc12Act_->setVisible(false);
    doc13Act_->setVisible(false);
    doc14Act_->setVisible(false);
    doc15Act_->setVisible(false);
    doc16Act_->setVisible(false);
    doc17Act_->setVisible(false);
    doc18Act_->setVisible(false);
    doc19Act_->setVisible(false);
    doc20Act_->setVisible(false);
    doc21Act_->setVisible(false);
    doc22Act_->setVisible(false);
    doc23Act_->setVisible(false);
    doc24Act_->setVisible(false);
    doc25Act_->setVisible(false);
    doc26Act_->setVisible(false);
    doc27Act_->setVisible(false);
    doc28Act_->setVisible(false);
    doc29Act_->setVisible(false);

    int index = 0;

    if (index >= Document::count()) {
        return;
    }
    doc00Act_->setVisible(true);
    doc00Act_->setText(
        QString("00: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc01Act_->setVisible(true);
    doc01Act_->setText(
        QString("01: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc02Act_->setVisible(true);
    doc02Act_->setText(
        QString("02: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc03Act_->setVisible(true);
    doc03Act_->setText(
        QString("03: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc04Act_->setVisible(true);
    doc04Act_->setText(
        QString("04: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc05Act_->setVisible(true);
    doc05Act_->setText(
        QString("05: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc06Act_->setVisible(true);
    doc06Act_->setText(
        QString("06: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc07Act_->setVisible(true);
    doc07Act_->setText(
        QString("07: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc08Act_->setVisible(true);
    doc08Act_->setText(
        QString("08: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc09Act_->setVisible(true);
    doc09Act_->setText(
        QString("09: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc10Act_->setVisible(true);
    doc10Act_->setText(
        QString("10: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc11Act_->setVisible(true);
    doc11Act_->setText(
        QString("11: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc12Act_->setVisible(true);
    doc12Act_->setText(
        QString("12: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc13Act_->setVisible(true);
    doc13Act_->setText(
        QString("13: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc14Act_->setVisible(true);
    doc14Act_->setText(
        QString("14: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc15Act_->setVisible(true);
    doc15Act_->setText(
        QString("15: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc16Act_->setVisible(true);
    doc16Act_->setText(
        QString("16: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc17Act_->setVisible(true);
    doc17Act_->setText(
        QString("17: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc18Act_->setVisible(true);
    doc18Act_->setText(
        QString("18: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc19Act_->setVisible(true);
    doc19Act_->setText(
        QString("19: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc20Act_->setVisible(true);
    doc20Act_->setText(
        QString("20: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc21Act_->setVisible(true);
    doc21Act_->setText(
        QString("21: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc22Act_->setVisible(true);
    doc22Act_->setText(
        QString("22: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc23Act_->setVisible(true);
    doc23Act_->setText(
        QString("23: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc24Act_->setVisible(true);
    doc24Act_->setText(
        QString("24: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc25Act_->setVisible(true);
    doc25Act_->setText(
        QString("25: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc26Act_->setVisible(true);
    doc26Act_->setText(
        QString("26: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc27Act_->setVisible(true);
    doc27Act_->setText(
        QString("27: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc28Act_->setVisible(true);
    doc28Act_->setText(
        QString("28: %1").arg(Document::document(index)->name()));
    ++index;

    if (index >= Document::count()) {
        return;
    }
    doc29Act_->setVisible(true);
    doc29Act_->setText(
        QString("29: %1").arg(Document::document(index)->name()));
    ++index;
}

void
MainWindow::updateActionCheckMarks()
{
    menuBarAct_->setIcon(isMenuBarVisible() ? QIcon(":/check.png") : QIcon());
    statusBarAct_->setIcon(
        isStatusBarVisible() ? QIcon(":/check.png") : QIcon());
    wordWrapAct_->setIcon(Editor::wordWrap() ? QIcon(":/check.png") : QIcon());
    spaceForTabsAct_->setIcon(
        Editor::isSpaceForTab() ? QIcon(":/check.png") : QIcon());
}

MainWindow&
MainWindow::get()
{
    return *mainWindow_;
}

void
MainWindow::newDocument()
{
    Document *doc = new Document(QString());
    if (Editor::activeEditor()) {
        Editor::activeEditor()->setDocument(doc);
    }
    updateDocumentsMenu();
    StatusBar::updateAllStatusBars();
}

void
MainWindow::openDocument()
{
    static QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setFilter(QDir::AllEntries | QDir::Hidden |
        QDir::NoDotAndDotDot);
    fileDialog->setDirectory(fileDirectory_);
    fileDialog->move(pos());
    fileDialog->resize(size());

    if (fileDialog->exec()) {
        fileDirectory_ = fileDialog->directory();
        openFiles(fileDialog->selectedFiles());
    }
}

void
MainWindow::quickOpen()
{
    QuickOpenDialog::get(this).setCurrentPath(
        fileDirectory_.absolutePath() + "/");
    if (QuickOpenDialog::get(this).exec()) {
        openFiles(QuickOpenDialog::get(this).paths());
        fileDirectory_.setPath(QuickOpenDialog::get(this).directory());
    }
}

void
MainWindow::closeDocument()
{
    Document *doc = dynamic_cast<Document*>(Editor::activeEditor()->document());
    QMessageBox::StandardButton button;

    if (doc && doc->needToSave() == true) {
        button = QMessageBox::warning(this, tr("Need to Save"),
            tr("This document has unsaved changes.\nWould you like to save it?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);
        if (button == QMessageBox::Save) {
            Editor::activeEditor()->saveDocument();
            Editor::activeEditor()->closeDocument();
        } else if (button == QMessageBox::Discard) {
            Editor::activeEditor()->closeDocument();
        }
    } else {
        Editor::activeEditor()->closeDocument();
    }
}

void
MainWindow::closeAll()
{
    int index = 0;
    Document *doc = 0;
    bool needSaving = false;

    while (index < Document::count()) {
        doc = Document::document(index);
        if (doc != 0 && doc->needToSave() == true) {
            needSaving = true;
            break;
        }
        ++index;
    }

    if (needSaving == true &&
            SaveListDialog::get().exec() != QDialog::Rejected) {
        index = Document::count();
        while (index > 0) {
            Editor::activeEditor()->closeDocument();
            --index;
        }
    }
}

void
MainWindow::saveDocument()
{
    if (((Document*) Editor::activeEditor()->document())->path().isEmpty()) {
        saveAs();
    } else if (!Editor::activeEditor()->saveDocument()) {
        QMessageBox::warning(this, tr("Cannot Save!"),
            QString(tr("Could not save %1!")).arg(
                ((Document*) Editor::activeEditor()->document())->name()));
    }
}

void
MainWindow::saveAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"),
        fileDirectory_.absolutePath());
    if (!filePath.isEmpty()) {
        fileDirectory_.setPath(filePath);
        if (!Editor::activeEditor()->saveAsDocument(filePath)) {
            QMessageBox::warning(this, tr("Cannot Save!"),
                QString(tr("Could not save %1!")).arg(
                    ((Document*) Editor::activeEditor()->document())->name()));
        }
    }
}

void
MainWindow::saveAll()
{
    Document *doc;
    QList<Document*> failures;
    QString text(tr("Could not save these documents:"));
    int index = 0;

    while (index < Document::count()) {
        doc = Document::document(index);
        if (!doc->save()) {
            failures.append(doc);
        }

        ++index;
    }

    if (!failures.isEmpty()) {
        index = 0;
        while (index < failures.count()) {
            text.append(QString("\n%1").arg(failures[index]->name()));
            ++index;
        }

        QMessageBox::warning(this, tr("Cannot Save!"), text);
    }
}

void
MainWindow::revertDocument()
{
    Editor::activeEditor()->revertDocument();
}

void
MainWindow::onPrint()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        Editor::activeEditor()->print(&printer);
    }
}

void
MainWindow::exitApplication()
{
    QString homeDir(getenv("HOME"));
    int index = 0;
    Document *doc = 0;
    bool needSaving = false;
    bool quit = true;

    while (index < Document::count()) {
        doc = Document::document(index);
        if (doc != 0 && doc->needToSave() == true) {
            needSaving = true;
            break;
        }
        ++index;
    }

    if (needSaving == true &&
            SaveListDialog::get().exec() == QDialog::Rejected) {
        quit = false;
    }

    if (quit == true) {
        homeDir += "/.config/codedit.xml";
        writePreferenceFile(homeDir);

        QApplication::closeAllWindows();
    }
}

void
MainWindow::undo()
{
    Editor::activeEditor()->undo();
}

void
MainWindow::redo()
{
    Editor::activeEditor()->redo();
}

void
MainWindow::cut()
{
    Editor::activeEditor()->cut();
}

void
MainWindow::copy()
{
    Editor::activeEditor()->copy();
}

void
MainWindow::paste()
{
    Editor::activeEditor()->paste();
}

void
MainWindow::selectAll()
{
    Editor::activeEditor()->selectAll();
}

void
MainWindow::searchReplace()
{
    if (SearchReplaceDialog::get().exec()) {
        Editor::activeEditor()->search();
    }
}

void
MainWindow::searchAgain()
{
    Editor::activeEditor()->search();
}

void
MainWindow::replaceCurrent()
{
    Editor::activeEditor()->replaceCurrent();
}

void
MainWindow::onJumpToLine()
{
    static int lastValue = 1;
    bool ok;

    int value = QInputDialog::getInt(this, tr("Jump to Line"),
        tr("Go to Line:"), lastValue, 1, 2147483647, 1, &ok);
    if (ok) {
        lastValue = value;
        Editor::activeEditor()->jumpToLine(value - 1);
    }
}

void
MainWindow::onJumpToPosition()
{
    static int lastValue = 0;
    bool ok;

    int value = QInputDialog::getInt(this, tr("Jump to Position"),
        tr("Go to Position:"), lastValue, 0, 2147483647, 1, &ok);
    if (ok) {
        lastValue = value;
        Editor::activeEditor()->jumpToPosition(value);
    }
}

void
MainWindow::indent()
{
    Editor::activeEditor()->indent();
}

void
MainWindow::unindent()
{
    Editor::activeEditor()->unindent();
}

void
MainWindow::comment()
{
    Editor::activeEditor()->comment();
}

void
MainWindow::wordWrap()
{
    Editor::setWordWrap(!Editor::wordWrap());
    wordWrapAct_->setIcon(Editor::wordWrap() ? QIcon(":/check.png") : QIcon());
}

void
MainWindow::onStatusBar()
{
    setStatusBarVisible(!isStatusBarVisible());
}

void
MainWindow::toggleMenuBar()
{
    setMenuBarVisible(!isMenuBarVisible());
}

void
MainWindow::spacesForTab()
{
    Editor::setSpaceForTab(!Editor::isSpaceForTab());
    spaceForTabsAct_->setIcon(
        Editor::isSpaceForTab() ? QIcon(":/check.png") : QIcon());
}

void
MainWindow::lineNumbers()
{
    Editor::setLineNumberEnabled(!Editor::isLineNumberEnabled());
    Editor::updateAllEditors();
}

void
MainWindow::createEditor()
{
    Editor *editor;
    for (int index = 0; index < Editor::count(); ++index) {
        editor = Editor::editor(index);
        if (!editor->isVisible()) {
            editor->setVisible(true);
            statusBarSplitter_->widget(index)->setVisible(true);
            Editor::setActiveEditor(editor);
            return;
        }
    }

    editor = new Editor;
    editorSplitter_->addWidget(editor);
    editor->setFocus();

    StatusBar *statusBar = new StatusBar(editor);
    editor->setStatusBar(statusBar);
    statusBarSplitter_->addWidget(statusBar);
    statusBar->updateDocumentList();
    statusBar->changeSelectedDocument(0);
}

void
MainWindow::removeEditor()
{
    Editor *editor;
    int index = Editor::count() - 1;

    while (index > 0) {
        editor = Editor::editor(index);
        if (editor->isVisible()) {
            editor->setVisible(false);
            statusBarSplitter_->widget(index)->setVisible(false);
            break;
        }

        --index;
    }

    if (index == 0) {
        index = 1;
    }

    Editor::setActiveEditor(Editor::editor(index - 1));
}

void
MainWindow::prevEditor()
{
    Editor *active = Editor::activeEditor();
    int activeIndex = 0;
    while (activeIndex < Editor::count()) {
        if (Editor::editor(activeIndex) == active) {
            break;
        }
        ++activeIndex;
    }

    int visCount = 0;
    while (visCount < Editor::count()) {
        if (!Editor::editor(visCount)->isVisible()) {
            break;
        }
        ++visCount;
    }

    if (activeIndex > 0) {
        --activeIndex;
    } else {
        activeIndex = visCount - 1;
    }

    active = Editor::editor(activeIndex);
    active->setFocus();
}

void
MainWindow::nextEditor()
{
    Editor *active = Editor::activeEditor();
    int activeIndex = 0;
    while (activeIndex < Editor::count()) {
        if (Editor::editor(activeIndex) == active) {
            break;
        }
        ++activeIndex;
    }

    int visCount = 0;
    while (visCount < Editor::count()) {
        if (!Editor::editor(visCount)->isVisible()) {
            break;
        }
        ++visCount;
    }

    if (activeIndex < visCount - 1) {
        ++activeIndex;
    } else {
        activeIndex = 0;
    }

    active = Editor::editor(activeIndex);
    active->setFocus();
}

void
MainWindow::onWindowTitle()
{
    bool ok;
    QString value = windowTitle();
    if (value.size() == 7) {
        value = QString();
    } else {
        value.remove(0, 9);
    }
    value = QInputDialog::getText(this, "Window Title",
        "Change the title of the window to:", QLineEdit::Normal, value, &ok);
    if (ok) {
        if (value.isEmpty()) {
            setWindowTitle("Codedit");
        } else {
            setWindowTitle(QString("Codedit: %1").arg(value));
        }
    }
}

void
MainWindow::preferences()
{
    PreferenceDialog::get(this).exec();
}

void
MainWindow::onFileInfo()
{
    FileInfoDialog::get(this).exec();
}

void
MainWindow::prevDocument()
{
    if (Document::count() == 1) {
        return;
    }

    int index = 0;
    Document *doc;
    Document *activeDoc = (Document*) Editor::activeEditor()->document();
    while (index < Document::count()) {
        doc = Document::document(index);
        if (doc == activeDoc) {
            break;
        }
        ++index;
    }

    if (index < Document::count()) {
        if (index == 0) {
            index = Document::count() - 1;
        } else {
            --index;
        }
        Editor::activeEditor()->selectDocument(index);
    }
}

void
MainWindow::nextDocument()
{
    if (Document::count() == 1) {
        return;
    }

    int index = 0;
    Document *doc;
    Document *activeDoc = (Document*) Editor::activeEditor()->document();
    while (index < Document::count()) {
        doc = Document::document(index);
        if (doc == activeDoc) {
            break;
        }
        ++index;
    }

    if (index < Document::count()) {
        if (index == Document::count() - 1) {
            index = 0;
        } else {
            ++index;
        }
        Editor::activeEditor()->selectDocument(index);
    }
}

void
MainWindow::document00()
{
    Editor::activeEditor()->selectDocument(0);
}

void
MainWindow::document01()
{
    Editor::activeEditor()->selectDocument(1);
}

void
MainWindow::document02()
{
    Editor::activeEditor()->selectDocument(2);
}

void
MainWindow::document03()
{
    Editor::activeEditor()->selectDocument(3);
}

void
MainWindow::document04()
{
    Editor::activeEditor()->selectDocument(4);
}

void
MainWindow::document05()
{
    Editor::activeEditor()->selectDocument(5);
}

void
MainWindow::document06()
{
    Editor::activeEditor()->selectDocument(6);
}

void
MainWindow::document07()
{
    Editor::activeEditor()->selectDocument(7);
}

void
MainWindow::document08()
{
    Editor::activeEditor()->selectDocument(8);
}

void
MainWindow::document09()
{
    Editor::activeEditor()->selectDocument(9);
}

void
MainWindow::document10()
{
    Editor::activeEditor()->selectDocument(10);
}

void
MainWindow::document11()
{
    Editor::activeEditor()->selectDocument(11);
}

void
MainWindow::document12()
{
    Editor::activeEditor()->selectDocument(12);
}

void
MainWindow::document13()
{
    Editor::activeEditor()->selectDocument(13);
}

void
MainWindow::document14()
{
    Editor::activeEditor()->selectDocument(14);
}

void
MainWindow::document15()
{
    Editor::activeEditor()->selectDocument(15);
}

void
MainWindow::document16()
{
    Editor::activeEditor()->selectDocument(16);
}

void
MainWindow::document17()
{
    Editor::activeEditor()->selectDocument(17);
}

void
MainWindow::document18()
{
    Editor::activeEditor()->selectDocument(18);
}

void
MainWindow::document19()
{
    Editor::activeEditor()->selectDocument(19);
}

void
MainWindow::document20()
{
    Editor::activeEditor()->selectDocument(20);
}

void
MainWindow::document21()
{
    Editor::activeEditor()->selectDocument(21);
}

void
MainWindow::document22()
{
    Editor::activeEditor()->selectDocument(22);
}

void
MainWindow::document23()
{
    Editor::activeEditor()->selectDocument(23);
}

void
MainWindow::document24()
{
    Editor::activeEditor()->selectDocument(24);
}

void
MainWindow::document25()
{
    Editor::activeEditor()->selectDocument(25);
}

void
MainWindow::document26()
{
    Editor::activeEditor()->selectDocument(26);
}

void
MainWindow::document27()
{
    Editor::activeEditor()->selectDocument(27);
}

void
MainWindow::document28()
{
    Editor::activeEditor()->selectDocument(28);
}

void
MainWindow::document29()
{
    Editor::activeEditor()->selectDocument(29);
}

void
MainWindow::onEditorSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);

    statusBarSplitter_->blockSignals(true);
    statusBarSplitter_->setSizes(editorSplitter_->sizes());
    statusBarSplitter_->blockSignals(false);
}

void
MainWindow::onStatusBarSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);

    editorSplitter_->blockSignals(true);
    editorSplitter_->setSizes(statusBarSplitter_->sizes());
    editorSplitter_->blockSignals(false);
}

void
MainWindow::onDelayedTimeout()
{
    openFiles(delayedFileOpenList_);
    delayedFileOpenList_.clear();
}

void
MainWindow::onDelayedSizing()
{
    switch (prefResult_) {
    case 0:
        move(position());
        resize(resolution());
        break;
    case 1:
        setWindowState(Qt::WindowMaximized);
        break;
    case 2:
        setWindowState(Qt::WindowFullScreen);
        break;
    }
}

void
MainWindow::createMenuBar()
{
    setMenuBar(menuBar_);
}

void
MainWindow::createFileMenu()
{
    QMenu *menu = new QMenu("&File");
    menuBar_->addMenu(menu);

    QAction *action;

    // New command.
    action = menu->addAction(tr("&New...", "New"));
    action->setWhatsThis(tr("Create a new text file.", "New Description"));
    action->setData("New");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    connect(action, SIGNAL(triggered()), this, SLOT(newDocument()));
    addAction(action);
    ActionEditor::addAction(action);

    // Open command.
    action = menu->addAction(tr("&Open...", "Open"));
    action->setWhatsThis(tr("Open a text file.", "Open Description"));
    action->setData("Open");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(action, SIGNAL(triggered()), this, SLOT(openDocument()));
    addAction(action);
    ActionEditor::addAction(action);

    // Quick Open command.
    action = menu->addAction(tr("Quick Open...", "Quick Open"));
    action->setWhatsThis(tr("Quickly open a text file using "
        "a simple dialog window.", "Quick Open Description"));
    action->setData("Quick Open");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
    connect(action, SIGNAL(triggered()), this, SLOT(quickOpen()));
    addAction(action);
    ActionEditor::addAction(action);

    // Close command.
    action = menu->addAction(tr("&Close", "Close"));
    action->setWhatsThis(tr("Close the active file.", "Close Description"));
    action->setData("Close");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(action, SIGNAL(triggered()), this, SLOT(closeDocument()));
    addAction(action);
    ActionEditor::addAction(action);

    // Close All command
    action = menu->addAction(tr("Close All", "Close All"));
    action->setWhatsThis(tr("Close all files.", "Close All Description"));
    action->setData("Close All");
    connect(action, SIGNAL(triggered()), this, SLOT(closeAll()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Save command.
    action = menu->addAction(tr("&Save", "Save"));
    action->setWhatsThis(tr("Save the active file.", "Save Description"));
    action->setData("Save");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(action, SIGNAL(triggered()), this, SLOT(saveDocument()));
    addAction(action);
    ActionEditor::addAction(action);

    // Save As command.
    action = menu->addAction(tr("Save As...", "Save As"));
    action->setWhatsThis(tr("Save the active file with a new name.",
        "Save As Description"));
    action->setData("Save As");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    connect(action, SIGNAL(triggered()), this, SLOT(saveAs()));
    addAction(action);
    ActionEditor::addAction(action);

    // Save All command.
    action = menu->addAction(tr("Save All", "Save All"));
    action->setWhatsThis(tr("Save all files.", "Save All Description"));
    action->setData("Save All");
    connect(action, SIGNAL(triggered()), this, SLOT(saveAll()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Revert command.
    action = menu->addAction(tr("&Revert", "Revert"));
    action->setWhatsThis(tr("Revert the current file to the last save.",
        "Revert Description"));
    action->setData("Revert");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(action, SIGNAL(triggered()), this, SLOT(revertDocument()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Print command.
    action = menu->addAction(tr("&Print", "Print"));
    action->setWhatsThis(tr("Print the current document.",
        "Print Description"));
    action->setData("Print");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    connect(action, SIGNAL(triggered()), this, SLOT(onPrint()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Exit command.
    action = menu->addAction(tr("E&xit", "Exit"));
    action->setWhatsThis(tr("Exit the application.", "Exit Description"));
    action->setData("Exit");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(action, SIGNAL(triggered()), this, SLOT(exitApplication()));
    addAction(action);
    ActionEditor::addAction(action);
}

void
MainWindow::createEditMenu()
{
    QMenu *menu = new QMenu("&Edit");
    menuBar_->addMenu(menu);

    QAction *action;

    // Undo command.
    action = menu->addAction(tr("&Undo", "Undo"));
    action->setWhatsThis(tr("Undo the last action. "
        "Multiple undoes supported.", "Undo Description"));
    action->setData("Undo");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    connect(action, SIGNAL(triggered()), this, SLOT(undo()));
    addAction(action);
    ActionEditor::addAction(action);

    // Redo command.
    action = menu->addAction(tr("&Redo", "Redo"));
    action->setWhatsThis(tr("Redo the last undone action. "
        "Multiple redoes supported.", "Redo Description"));
    action->setData("Redo");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    connect(action, SIGNAL(triggered()), this, SLOT(redo()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Cut command.
    action = menu->addAction(tr("Cu&t", "Cut"));
    action->setWhatsThis(tr("Cut the selected text "
        "in the active editor.", "Cut Description"));
    action->setData("Cut");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    connect(action, SIGNAL(triggered()), this, SLOT(cut()));
    addAction(action);
    ActionEditor::addAction(action);

    // Copy command.
    action = menu->addAction(tr("&Copy", "Copy"));
    action->setWhatsThis(tr("Copy the selected text "
        "in the active editor.", "Copy Description"));
    action->setData("Copy");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    connect(action, SIGNAL(triggered()), this, SLOT(paste()));
    addAction(action);
    ActionEditor::addAction(action);

    // Paste command.
    action = menu->addAction(tr("&Paste", "Paste"));
    action->setWhatsThis(tr("Paste the selected text "
        "in the active editor.", "Paste Description"));
    action->setData("Paste");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Select All command.
    action = menu->addAction(tr("Select &All", "Select All"));
    action->setWhatsThis(tr("Select all the text "
        "in the active editor.", "Select All Description"));
    action->setData("Select All");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(action, SIGNAL(triggered()), this, SLOT(selectAll()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Search command.
    action = menu->addAction(tr("&Search/Replace...", "Search"));
    action->setWhatsThis(tr("Open the search dialog to search for text "
        "in the active editor.", "Search Description"));
    action->setData("Search");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    connect(action, SIGNAL(triggered()), this, SLOT(searchReplace()));
    addAction(action);
    ActionEditor::addAction(action);

    // Search Again command.
    action = menu->addAction(tr("Search A&gain",
        "Search Again"));
    action->setWhatsThis(tr("Search again for the current search "
        "string in the active file.", "Search Again Description"));
    action->setData("Search Again");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    connect(action, SIGNAL(triggered()), this, SLOT(searchAgain()));
    addAction(action);
    ActionEditor::addAction(action);

    // Replace Current command.
    action = menu->addAction(tr("R&eplace Current",
        "Replace Current"));
    action->setWhatsThis(tr("", "Replace Current Description"));
    action->setData("Replace Current");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    connect(action, SIGNAL(triggered()), this, SLOT(replaceCurrent()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Jump to Line command.
    action = menu->addAction(tr("&Jump to Line...", "Jump"));
    action->setWhatsThis(tr("Jump to a line in the active editor.",
        "Jump Description"));
    action->setData("Jump");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));
    connect(action, SIGNAL(triggered()), this, SLOT(onJumpToLine()));
    addAction(action);
    ActionEditor::addAction(action);

    // Jump to Position command.
    action = menu->addAction(tr("&Jump to Position...", "JumpToPosition"));
    action->setWhatsThis(tr("Jump to a position in the active editor.",
        "Jump to Position Description"));
    action->setData("JumpToPosition");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_J));
    connect(action, SIGNAL(triggered()), this, SLOT(onJumpToPosition()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Indent command.
    action = menu->addAction(tr("&Indent", "Indent"));
    action->setWhatsThis(tr("Indents the selected lines in the active file.",
        "Indent Description"));
    action->setData("Indent");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    connect(action, SIGNAL(triggered()), this, SLOT(indent()));
    addAction(action);
    ActionEditor::addAction(action);

    // Unindent command.
    action = menu->addAction(tr("&Unindent", "Unindent"));
    action->setWhatsThis(tr("Unindents the selected lines "
        "in the active file.", "Unindent Description"));
    action->setData("Unindent");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
    connect(action, SIGNAL(triggered()), this, SLOT(unindent()));
    addAction(action);
    ActionEditor::addAction(action);

    // Comment/Uncomment command.
    action = menu->addAction(tr("Co&mment/Uncomment", "Comment"));
    action->setWhatsThis(tr("Comments or uncomments the line using the single "
        "line comment. If only a multiline comment is available, each "
        "individual line is commented separately."));
    action->setData("Comment");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Slash));
    connect(action, SIGNAL(triggered()), this, SLOT(comment()));
    addAction(action);
    ActionEditor::addAction(action);
}

void
MainWindow::createOptionsMenu()
{
    QMenu *menu = new QMenu("&Options");
    QAction *action;
    menuBar_->addMenu(menu);

    // Word Wrap option.
    wordWrapAct_ = menu->addAction(tr("&Word Wrap", "Word Wrap"));
    wordWrapAct_->setWhatsThis(tr("Toggle whether words are wrapped "
        "to the next line.", "Word Wrap Description"));
    wordWrapAct_->setData("Word Wrap");
    wordWrapAct_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    wordWrapAct_->setIconVisibleInMenu(true);
    connect(wordWrapAct_, SIGNAL(triggered()), this, SLOT(wordWrap()));
    addAction(wordWrapAct_);
    ActionEditor::addAction(wordWrapAct_);

    // Status Bar visibility.
    statusBarAct_ = menu->addAction(tr("S&tatus Bar", "Status Bar"));
    statusBarAct_->setWhatsThis(tr("Toggle the visibility of the status bar.",
        "Status Bar Description"));
    statusBarAct_->setData("Status Bar");
    statusBarAct_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    statusBarAct_->setIconVisibleInMenu(true);
    connect(statusBarAct_, SIGNAL(triggered()), this, SLOT(onStatusBar()));
    addAction(statusBarAct_);
    ActionEditor::addAction(statusBarAct_);

    // Menu Bar visibility.
    menuBarAct_ = menu->addAction(tr("&Menu Bar", "Menu Bar"));
    menuBarAct_->setWhatsThis(tr("Toggle the visibility of the menu bar.",
        "Menu Bar Description"));
    menuBarAct_->setData("Menu Bar");
    menuBarAct_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    menuBarAct_->setIconVisibleInMenu(true);
    connect(menuBarAct_, SIGNAL(triggered()), this, SLOT(toggleMenuBar()));
    addAction(menuBarAct_);
    ActionEditor::addAction(menuBarAct_);

    // Tab Spaces option.
    spaceForTabsAct_ = menu->addAction(
        tr("&Spaces for Tabs", "Spaces for Tabs"));
    spaceForTabsAct_->setWhatsThis(tr("Toggles whether the tab key will "
        "produce a tab character or spaces.", "Spaces for Tabs Description"));
    spaceForTabsAct_->setData("Spaces for Tabs");
    spaceForTabsAct_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Comma));
    spaceForTabsAct_->setIconVisibleInMenu(true);
    connect(spaceForTabsAct_, SIGNAL(triggered()), this,
        SLOT(spacesForTab()));
    addAction(spaceForTabsAct_);
    ActionEditor::addAction(spaceForTabsAct_);

    // Line Numbers.
    lineNumbersAct_ = menu->addAction(tr("&Line Numbers", "Line Number"));
    lineNumbersAct_->setWhatsThis(tr("Toggle the visibility "
        "of the line numbers.", "Line Number Description"));
    lineNumbersAct_->setData("Line Number");
    lineNumbersAct_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    lineNumbersAct_->setIconVisibleInMenu(true);
    connect(lineNumbersAct_, SIGNAL(triggered()), this,
        SLOT(lineNumbers()));
    addAction(lineNumbersAct_);
    ActionEditor::addAction(lineNumbersAct_);

    menu->addSeparator();

    action = menu->addAction(tr("Add Editor", "Add Editor"));
    action->setWhatsThis(tr("Add an editor.", "Add Editor Description"));
    action->setData("Add Editor");
    action->setShortcut(Qt::CTRL + Qt::Key_Equal);
    connect(action, SIGNAL(triggered()), this, SLOT(createEditor()));
    addAction(action);
    ActionEditor::addAction(action);

    action = menu->addAction(tr("Remove Editor", "Remove Editor"));
    action->setWhatsThis(tr("Remove an editor.", "Remove Editor Description"));
    action->setData("Remove Editor");
    action->setShortcut(Qt::CTRL + Qt::Key_Minus);
    connect(action, SIGNAL(triggered()), this, SLOT(removeEditor()));
    addAction(action);
    ActionEditor::addAction(action);

    action = menu->addAction(tr("Next Editor", "Next Editor"));
    action->setWhatsThis(tr("Switch to the next editor.",
        "Next Editor Description"));
    action->setData("Next Editor");
    action->setShortcut(Qt::CTRL + Qt::Key_Tab);
    connect(action, SIGNAL(triggered()), this, SLOT(nextEditor()));
    addAction(action);
    ActionEditor::addAction(action);

    action = menu->addAction(tr("Previous Editor", "Prev Editor"));
    action->setWhatsThis(tr("Switch to the previous editor.",
        "Prev Editor Description"));
    action->setData("Prev Editor");
    action->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab);
    connect(action, SIGNAL(triggered()), this, SLOT(prevEditor()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Window title dialog.
    action = menu->addAction(tr("&Window Title...", "Window Title"));
    action->setWhatsThis(tr("Open the dialog to change the window's title.",
        "Window Title Description"));
    action->setData("Window Title");
    action->setShortcut(Qt::CTRL + Qt::Key_Apostrophe);
    connect(action, SIGNAL(triggered()), this, SLOT(onWindowTitle()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // File info dialog.
    action = menu->addAction(tr("&File Info...", "File Info"));
    action->setWhatsThis(tr("Opens the dialog to change the document name, "
        "category, or its file path.", "File Info Description"));
    action->setData("File Info");
    action->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(action, SIGNAL(triggered()), this, SLOT(onFileInfo()));
    addAction(action);
    ActionEditor::addAction(action);

    menu->addSeparator();

    // Preferences dialog.
    action = menu->addAction(tr("&Preferences...", "Preferences"));
    action->setWhatsThis(tr("Open the preferences dialog window.",
        "Preferences Description"));
    action->setData("Preferences");
    action->setShortcut(Qt::CTRL + Qt::Key_Semicolon);
    connect(action, SIGNAL(triggered()), this, SLOT(preferences()));
    addAction(action);
    ActionEditor::addAction(action);
}

void
MainWindow::createDocumentsMenu()
{
    QAction *action;
    menuBar_->addMenu(documentsMenu_);

    // Previous Document.
    action = documentsMenu_->addAction(tr("&Previous Document",
        "Previous Document"));
    action->setWhatsThis(tr("Switch to the previous document in the "
        "active editor.", "Previous Document Description"));
    action->setData("Previous Document");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_BracketLeft));
    connect(action, SIGNAL(triggered()), this, SLOT(prevDocument()));
    addAction(action);
    ActionEditor::addAction(action);

    // Next Document.
    action = documentsMenu_->addAction(tr("&Next Document", "Next Document"));
    action->setWhatsThis(tr("Switch to the next document in the active editor.",
        "Next Document Description"));
    action->setData("Next Document");
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_BracketRight));
    connect(action, SIGNAL(triggered()), this, SLOT(nextDocument()));
    addAction(action);
    ActionEditor::addAction(action);

    documentsMenu_->addSeparator();

    // Document 0.
    doc00Act_ = documentsMenu_->addAction("");
    doc00Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    connect(doc00Act_, SIGNAL(triggered()), this, SLOT(document00()));
    doc00Act_->setVisible(false);
    addAction(doc00Act_);

    // Document 1.
    doc01Act_ = documentsMenu_->addAction("");
    doc01Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    connect(doc01Act_, SIGNAL(triggered()), this, SLOT(document01()));
    doc01Act_->setVisible(false);
    addAction(doc01Act_);

    // Document 2.
    doc02Act_ = documentsMenu_->addAction("");
    doc02Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    connect(doc02Act_, SIGNAL(triggered()), this, SLOT(document02()));
    doc02Act_->setVisible(false);
    addAction(doc02Act_);

    // Document 3.
    doc03Act_ = documentsMenu_->addAction("");
    doc03Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
    connect(doc03Act_, SIGNAL(triggered()), this, SLOT(document03()));
    doc03Act_->setVisible(false);
    addAction(doc03Act_);

    // Document 4.
    doc04Act_ = documentsMenu_->addAction("");
    doc04Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
    connect(doc04Act_, SIGNAL(triggered()), this, SLOT(document04()));
    doc04Act_->setVisible(false);
    addAction(doc04Act_);

    // Document 5.
    doc05Act_ = documentsMenu_->addAction("");
    doc05Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));
    connect(doc05Act_, SIGNAL(triggered()), this, SLOT(document05()));
    doc05Act_->setVisible(false);
    addAction(doc05Act_);

    // Document 6.
    doc06Act_ = documentsMenu_->addAction("");
    doc06Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_6));
    connect(doc06Act_, SIGNAL(triggered()), this, SLOT(document06()));
    doc06Act_->setVisible(false);
    addAction(doc06Act_);

    // Document 7.
    doc07Act_ = documentsMenu_->addAction("");
    doc07Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_7));
    connect(doc07Act_, SIGNAL(triggered()), this, SLOT(document07()));
    doc07Act_->setVisible(false);
    addAction(doc07Act_);

    // Document 8.
    doc08Act_ = documentsMenu_->addAction("");
    doc08Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_8));
    connect(doc08Act_, SIGNAL(triggered()), this, SLOT(document08()));
    doc08Act_->setVisible(false);
    addAction(doc08Act_);

    // Document 9.
    doc09Act_ = documentsMenu_->addAction("");
    doc09Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_9));
    connect(doc09Act_, SIGNAL(triggered()), this, SLOT(document09()));
    doc09Act_->setVisible(false);
    addAction(doc09Act_);

    // Document 10.
    doc10Act_ = documentsMenu_->addAction("");
    doc10Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_0));
    connect(doc10Act_, SIGNAL(triggered()), this, SLOT(document10()));
    doc10Act_->setVisible(false);
    addAction(doc10Act_);

    // Document 11.
    doc11Act_ = documentsMenu_->addAction("");
    doc11Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_1));
    connect(doc11Act_, SIGNAL(triggered()), this, SLOT(document11()));
    doc11Act_->setVisible(false);
    addAction(doc11Act_);

    // Document 12.
    doc12Act_ = documentsMenu_->addAction("");
    doc12Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_2));
    connect(doc12Act_, SIGNAL(triggered()), this, SLOT(document12()));
    doc12Act_->setVisible(false);
    addAction(doc12Act_);

    // Document 13.
    doc13Act_ = documentsMenu_->addAction("");
    doc13Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_3));
    connect(doc13Act_, SIGNAL(triggered()), this, SLOT(document13()));
    doc13Act_->setVisible(false);
    addAction(doc13Act_);

    // Document 14.
    doc14Act_ = documentsMenu_->addAction("");
    doc14Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_4));
    connect(doc14Act_, SIGNAL(triggered()), this, SLOT(document14()));
    doc14Act_->setVisible(false);
    addAction(doc14Act_);

    // Document 15.
    doc15Act_ = documentsMenu_->addAction("");
    doc15Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_5));
    connect(doc15Act_, SIGNAL(triggered()), this, SLOT(document15()));
    doc15Act_->setVisible(false);
    addAction(doc15Act_);

    // Document 16.
    doc16Act_ = documentsMenu_->addAction("");
    doc16Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_6));
    connect(doc16Act_, SIGNAL(triggered()), this, SLOT(document16()));
    doc16Act_->setVisible(false);
    addAction(doc16Act_);

    // Document 17.
    doc17Act_ = documentsMenu_->addAction("");
    doc17Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_7));
    connect(doc17Act_, SIGNAL(triggered()), this, SLOT(document17()));
    doc17Act_->setVisible(false);
    addAction(doc17Act_);

    // Document 18.
    doc18Act_ = documentsMenu_->addAction("");
    doc18Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_8));
    connect(doc18Act_, SIGNAL(triggered()), this, SLOT(document18()));
    doc18Act_->setVisible(false);
    addAction(doc18Act_);

    // Document 19.
    doc19Act_ = documentsMenu_->addAction("");
    doc19Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_9));
    connect(doc19Act_, SIGNAL(triggered()), this, SLOT(document19()));
    doc19Act_->setVisible(false);
    addAction(doc19Act_);

    // Document 20.
    doc20Act_ = documentsMenu_->addAction("");
    doc20Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_0));
    connect(doc20Act_, SIGNAL(triggered()), this, SLOT(document20()));
    doc20Act_->setVisible(false);
    addAction(doc20Act_);

    // Document 21.
    doc21Act_ = documentsMenu_->addAction("");
    doc21Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_1));
    connect(doc21Act_, SIGNAL(triggered()), this, SLOT(document21()));
    doc21Act_->setVisible(false);
    addAction(doc21Act_);

    // Document 22.
    doc22Act_ = documentsMenu_->addAction("");
    doc22Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_2));
    connect(doc22Act_, SIGNAL(triggered()), this, SLOT(document22()));
    doc22Act_->setVisible(false);
    addAction(doc22Act_);

    // Document 23.
    doc23Act_ = documentsMenu_->addAction("");
    doc23Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_3));
    connect(doc23Act_, SIGNAL(triggered()), this, SLOT(document23()));
    doc23Act_->setVisible(false);
    addAction(doc23Act_);

    // Document 24.
    doc24Act_ = documentsMenu_->addAction("");
    doc24Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_4));
    connect(doc24Act_, SIGNAL(triggered()), this, SLOT(document24()));
    doc24Act_->setVisible(false);
    addAction(doc24Act_);

    // Document 25.
    doc25Act_ = documentsMenu_->addAction("");
    doc25Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_5));
    connect(doc25Act_, SIGNAL(triggered()), this, SLOT(document25()));
    doc25Act_->setVisible(false);
    addAction(doc25Act_);

    // Document 26.
    doc26Act_ = documentsMenu_->addAction("");
    doc26Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_6));
    connect(doc26Act_, SIGNAL(triggered()), this, SLOT(document26()));
    doc26Act_->setVisible(false);
    addAction(doc26Act_);

    // Document 27.
    doc27Act_ = documentsMenu_->addAction("");
    doc27Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_7));
    connect(doc27Act_, SIGNAL(triggered()), this, SLOT(document27()));
    doc27Act_->setVisible(false);
    addAction(doc27Act_);

    // Document 28.
    doc28Act_ = documentsMenu_->addAction("");
    doc28Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_8));
    connect(doc28Act_, SIGNAL(triggered()), this, SLOT(document28()));
    doc28Act_->setVisible(false);
    addAction(doc28Act_);

    // Document 29.
    doc29Act_ = documentsMenu_->addAction("");
    doc29Act_->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_9));
    connect(doc29Act_, SIGNAL(triggered()), this, SLOT(document29()));
    doc29Act_->setVisible(false);
    addAction(doc29Act_);
}

void
MainWindow::openFiles(const QStringList& files)
{
    Document *firstOpenDocument = NULL;
    Document *doc;
    QList<QString>::const_iterator iter = files.constBegin();

    while (iter != files.constEnd()) {
        doc = openFile(*iter);
        if (firstOpenDocument == NULL) {
            firstOpenDocument = doc;
        }

        ++iter;
    }

    if (firstOpenDocument != NULL) {
        updateDocumentsMenu();
        if (Editor::activeEditor() != NULL) {
            Editor::activeEditor()->setDocument(firstOpenDocument);
        }
    }

    StatusBar::updateAllStatusBars();
}

Document*
MainWindow::openFile(const QString& file)
{
    Document *result = NULL;
    int index = Document::count() - 1;
    QFileInfo fileInfo(file);
    QString openPath = fileInfo.absoluteFilePath();
    QString docPath;

    if (!fileInfo.isDir()) {
        while (index >= 0) {
            docPath =
                QFileInfo(Document::document(index)->path()).absoluteFilePath();
            if (openPath == docPath) {
                break;
            }

            --index;
        }

        if (index < 0) {
            result = new Document(file);
            result->open();
        } else {
            result = Document::document(index);
        }
    }

    return result;
}

