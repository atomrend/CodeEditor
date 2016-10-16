#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDir>
#include <QMainWindow>
class Document;
class QAction;
class QMenu;
class QMenuBar;
class QSplitter;
class QStatusBar;

class MainWindow :
    public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const QStringList& files, QWidget* parent = 0,
        Qt::WindowFlags f = 0);
    ~MainWindow();

    bool isMenuBarVisible() const;
    void setMenuBarVisible(bool vis = true);

    bool isStatusBarVisible() const;
    void setStatusBarVisible(bool vis = true);

    void updateDocumentsMenu();

    void updateActionCheckMarks();

    static MainWindow& get();

protected slots:
    // File menu.
    void newDocument();
    void openDocument();
    void quickOpen();
    void closeDocument();
    void closeAll();
    void saveDocument();
    void saveAs();
    void saveAll();
    void revertDocument();
    void onPrint();
    void exitApplication();

    // Edit menu.
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void searchReplace();
    void searchAgain();
    void replaceCurrent();
    void onJumpToLine();
    void onJumpToPosition();
    void indent();
    void unindent();
    void comment();

    // Options menu.
    void wordWrap();
    void onStatusBar();
    void toggleMenuBar();
    void spacesForTab();
    void lineNumbers();
    void createEditor();
    void removeEditor();
    void prevEditor();
    void nextEditor();
    void onWindowTitle();
    void preferences();
    void onFileInfo();

    // Documents menu.
    void prevDocument();
    void nextDocument();
    void document00();
    void document01();
    void document02();
    void document03();
    void document04();
    void document05();
    void document06();
    void document07();
    void document08();
    void document09();
    void document10();
    void document11();
    void document12();
    void document13();
    void document14();
    void document15();
    void document16();
    void document17();
    void document18();
    void document19();
    void document20();
    void document21();
    void document22();
    void document23();
    void document24();
    void document25();
    void document26();
    void document27();
    void document28();
    void document29();

    void onEditorSplitterMoved(int pos, int index);
    void onStatusBarSplitterMoved(int pos, int index);

    void onDelayedTimeout();
    void onDelayedSizing();

protected:
    void createMenuBar();
    void createFileMenu();
    void createEditMenu();
    void createOptionsMenu();
    void createDocumentsMenu();
    void openFiles(const QStringList& files);
    Document* openFile(const QString& file);

private:
    QSplitter *editorSplitter_;
    QStatusBar *statusBar_;
    QSplitter *statusBarSplitter_;
    QMenuBar *menuBar_;
    QMenu *documentsMenu_;
    QAction *wordWrapAct_;
    QAction *statusBarAct_;
    QAction *menuBarAct_;
    QAction *spaceForTabsAct_;
    QAction *lineNumbersAct_;
    QAction *doc00Act_;
    QAction *doc01Act_;
    QAction *doc02Act_;
    QAction *doc03Act_;
    QAction *doc04Act_;
    QAction *doc05Act_;
    QAction *doc06Act_;
    QAction *doc07Act_;
    QAction *doc08Act_;
    QAction *doc09Act_;
    QAction *doc10Act_;
    QAction *doc11Act_;
    QAction *doc12Act_;
    QAction *doc13Act_;
    QAction *doc14Act_;
    QAction *doc15Act_;
    QAction *doc16Act_;
    QAction *doc17Act_;
    QAction *doc18Act_;
    QAction *doc19Act_;
    QAction *doc20Act_;
    QAction *doc21Act_;
    QAction *doc22Act_;
    QAction *doc23Act_;
    QAction *doc24Act_;
    QAction *doc25Act_;
    QAction *doc26Act_;
    QAction *doc27Act_;
    QAction *doc28Act_;
    QAction *doc29Act_;

    QDir fileDirectory_;
    QStringList delayedFileOpenList_;
    int prefResult_;

    static MainWindow *mainWindow_;
};

#endif // MAIN_WINDOW_H