#ifndef EDITOR_H
#define EDITOR_H

#include "Document.h"
#include "MainWindow.h"
#include <QColor>
#include <QFont>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QVector>
#include "StatusBar.h"
class QEvent;
class QKeyEvent;
class QPaintEvent;
class QRect;
class QResizeEvent;
class QScrollBar;

class Editor :
    public QPlainTextEdit
{
    Q_OBJECT
public:
    Editor(QWidget* parent = 0);
    virtual ~Editor();

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

    void closeDocument();
    bool saveDocument();
    bool saveAsDocument(const QString& path);
    void revertDocument();
    void search();
    void replaceCurrent();
    void indent();
    void unindent();
    void comment();
    void jumpToLine(int val);
    void jumpToPosition(int val);

    void setDocument(Document* doc);

    void setStatusBar(StatusBar* statusBar);

    static const QColor& foregroundColor();
    static void setForegroundColor(const QColor& color);

    static const QColor& backgroundColor();
    static void setBackgroundColor(const QColor& color);

    static const QColor& nondocumentColor();
    static void setNondocumentColor(const QColor& color);

    static const QColor& char80IndicatorColor();
    static void setChar80IndicatorColor(const QColor& color);

    static const QColor& highlightColor();
    static void setHighlightColor(const QColor& color);

    static const QColor& currentLineColor();
    static void setCurrentLineColor(const QColor& color);

    static const QColor& activeLineNumberForegroundColor();
    static void setActiveLineNumberForegroundColor(const QColor& color);

    static const QColor& inactiveLineNumberForegroundColor();
    static void setInactiveLineNumberForegroundColor(const QColor& color);

    static const QColor& activeLineNumberBackgroundColor();
    static void setActiveLineNumberBackgroundColor(const QColor& color);

    static const QColor& inactiveLineNumberBackgroundColor();
    static void setInactiveLineNumberBackgroundColor(const QColor& color);

    static const QColor& lineNumberBorderColor();
    static void setLineNumberBorderColor(const QColor& color);

    static bool isLineNumberBorderEnabled();
    static void setLineNumberBorderEnabled(bool val = true);

    static bool isLineNumberEnabled();
    static void setLineNumberEnabled(bool val = true);

    static bool isSpaceForTab();
    static void setSpaceForTab(bool val = true);

    static bool wordWrap();
    static void setWordWrap(bool val = false);

    static int tabSize();
    static void setTabSize(int size = 4);

    static const QFont& editorFont();
    static void setEditorFont(const QFont& font);

    static void updateAllEditors();

    static Editor* activeEditor();
    static void setActiveEditor(Editor* editor);
    static int count();
    static Editor* editor(int index);

public slots:
    void updateLineNumberAreaWidth(int);
    void selectDocument(int index);

protected slots:
    void updateLineNumberArea(const QRect&, int);
    void updateCursorPosition();
    void onScrollBarValueChanged(int value);
    void onUpdateRequest(const QRect& rect, int dy);

protected:
    void keyPressEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent* event);
    bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    void selectedBlocks(int& startBlock, int& endBlock, int& startPos,
        int& endPos);
    static void resetAllPositions(Editor* excluded = NULL);

private:
    QWidget *lineNumberArea_;
    StatusBar *statusBar_;

    static QVector<Editor*> editors_;
    static Editor *activeEditor_;
    static QColor foregroundColor_;
    static QColor backgroundColor_;
    static QColor nondocumentColor_;
    static QColor char80Indicator_;
    static QColor highlightColor_;
    static QColor currentLineColor_;
    static QColor activeLineNumberForegroundColor_;
    static QColor inactiveLineNumberForegroundColor_;
    static QColor activeLineNumberBackgroundColor_;
    static QColor inactiveLineNumberBackgroundColor_;
    static QColor lineNumberBorderColor_;
    static bool lineNumberBorderEnabled_;
    static bool lineNumberEnabled_;
    static bool spaceForTab_;
    static bool wordWrap_;
    static int tabSize_;
    static QFont font_;
    static bool positionUpdateEnabled_;
    static int char80Width_;
};

#ifdef INLINE
#include "Editor.icc"
#endif // INLINE

#endif // EDITOR_H