#include "Editor.h"
#include "Document.h"
#include "PreferenceDialog.h"
#include <QFontMetrics>
#include <QKeyEvent>
#include <QList>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextBlock>
#include "SearchReplaceDialog.h"
#include "StatusBar.h"
#include "SyntaxHighlighter.h"

QVector<Editor*> Editor::editors_ = QVector<Editor*>();
Editor* Editor::activeEditor_ = NULL;
QColor Editor::foregroundColor_ = QColor(Qt::black);
QColor Editor::backgroundColor_ = QColor(Qt::white);
QColor Editor::nondocumentColor_ = QColor(127, 127, 127);
QColor Editor::char80Indicator_ = QColor(239, 239, 239);
QColor Editor::highlightColor_ = QColor(0, 0, 128);
QColor Editor::currentLineColor_ = QColor(255, 255, 153);
QColor Editor::activeLineNumberForegroundColor_ = QColor(Qt::black);
QColor Editor::inactiveLineNumberForegroundColor_ = QColor(Qt::black);
QColor Editor::activeLineNumberBackgroundColor_ = QColor(191, 191, 191);
QColor Editor::inactiveLineNumberBackgroundColor_ = QColor(Qt::white);
QColor Editor::lineNumberBorderColor_ = QColor(Qt::black);
bool Editor::lineNumberBorderEnabled_ = true;
bool Editor::lineNumberEnabled_ = true;
bool Editor::spaceForTab_ = false;
bool Editor::wordWrap_ = false;
int Editor::tabSize_ = 4;
QFont Editor::font_ = QFont("Monospace", 10);
bool Editor::positionUpdateEnabled_ = true;
int Editor::char80Width_ = 641;

// ############################################################################
// LineNumberArea
// ############################################################################

class LineNumberArea :
    public QWidget
{
public:
    LineNumberArea(Editor* editor);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event);

private:
    Editor *editor_;
};

LineNumberArea::LineNumberArea(Editor* editor) :
    QWidget(editor),
    editor_(editor)
{
}

QSize
LineNumberArea::sizeHint() const
{
    return QSize(editor_->lineNumberAreaWidth(), 0);
}

void
LineNumberArea::paintEvent(QPaintEvent* event)
{
    editor_->lineNumberAreaPaintEvent(event);
}

// #############################################################################
// Editor
// #############################################################################
Editor::Editor(QWidget* parent) :
    QPlainTextEdit(parent),
    lineNumberArea_(new LineNumberArea(this)),
    statusBar_(NULL)
{
    editors_.append(this);

    setTabStopWidth(QFontMetrics(font_).width(" ") * tabSize_);
    setWordWrapMode(wordWrap_ ? QTextOption::WordWrap : QTextOption::NoWrap);
    setBackgroundVisible(true);

    QPalette pal(verticalScrollBar()->palette());
    QColor scrollWin = pal.color(QPalette::Window);
    QColor scrollBase = pal.color(QPalette::Base);

    pal = palette();
    pal.setColor(QPalette::Window, nondocumentColor_);
    pal.setColor(QPalette::Text, foregroundColor_);
    pal.setColor(QPalette::Base, backgroundColor_);
    pal.setColor(QPalette::Highlight, highlightColor_);
    setPalette(pal);

    pal = verticalScrollBar()->palette();
    pal.setColor(QPalette::Window, scrollWin);
    pal.setColor(QPalette::Base, scrollBase);
    verticalScrollBar()->setPalette(pal);
    horizontalScrollBar()->setPalette(pal);

    connect(this, SIGNAL(blockCountChanged(int)),
        this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(const QRect&, int)),
        this, SLOT(updateLineNumberArea(const QRect&, int)));
    connect(this, SIGNAL(cursorPositionChanged()),
        this, SLOT(updateCursorPosition()));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
        this, SLOT(onScrollBarValueChanged(int)));
    connect(this, SIGNAL(updateRequest(const QRect&, int)),
        this, SLOT(onUpdateRequest(const QRect&, int)));

    if (Document::count() > 0) {
        setDocument(Document::document(0));
    }
    updateLineNumberAreaWidth(0);

    if (activeEditor_ == NULL) {
        activeEditor_ = this;
    }
}

Editor::~Editor()
{
    if (editors_.contains(this)) {
        editors_.remove(editors_.indexOf(this));
    }
}

void
Editor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    if (!isLineNumberEnabled()) {
        return;
    }

    QPainter painter(lineNumberArea_);
    QRect rect(event->rect());

    // Background color.
    if (activeEditor() == this) {
        painter.fillRect(rect, activeLineNumberBackgroundColor());
    } else {
        painter.fillRect(rect, inactiveLineNumberBackgroundColor());
    }

    // Border.
    if (isLineNumberBorderEnabled()) {
        painter.setPen(lineNumberBorderColor());
        painter.drawLine(rect.x() + rect.width() - 1, rect.y(),
            rect.x() + rect.width() - 1, rect.y() + rect.height());
    }

    // Text.
    if (activeEditor() == this) {
        painter.setPen(activeLineNumberForegroundColor());
    } else {
        painter.setPen(inactiveLineNumberForegroundColor());
    }

    painter.setFont(Editor::editorFont());
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top =
        (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= rect.bottom()) {
        if (block.isVisible() && bottom >= rect.top()) {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(-1, top, lineNumberArea_->width(),
                fontMetrics().height(), Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int
Editor::lineNumberAreaWidth()
{
    if (!isLineNumberEnabled()) {
        return 0;
    }

    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    QFontMetrics fontMet(editorFont());
    int space = fontMet.width(QLatin1Char('9')) * digits + 2;

    return space;
}

void
Editor::search()
{
    QString docText(document()->toPlainText());
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    int resultPos = -1;
    bool direction = SearchReplaceDialog::get().searchDirectionForward();
    QString searchString = SearchReplaceDialog::get().searchString();
    Qt::CaseSensitivity caseSense =
        SearchReplaceDialog::get().searchCaseSensitive() ? Qt::CaseSensitive :
            Qt::CaseInsensitive;

    if (direction) {
        resultPos = docText.indexOf(searchString, pos, caseSense);
    } else {
        resultPos = docText.lastIndexOf(searchString, pos - 1, caseSense);
    }

    if (resultPos == -1) {
        // Nothing found. Try wrapping around document.
        if (direction) {
            pos = 0;
            resultPos = docText.indexOf(searchString, pos, caseSense);
        } else {
            pos = -1;
            resultPos = docText.lastIndexOf(searchString, pos, caseSense);
        }
    }

    if (resultPos != -1) {
        if (direction) {
            cursor.setPosition(resultPos);
            cursor.setPosition(resultPos + searchString.length(),
                QTextCursor::KeepAnchor);
        } else {
            cursor.setPosition(resultPos + searchString.length());
            cursor.setPosition(resultPos, QTextCursor::KeepAnchor);
        }
    }
    setTextCursor(cursor);

    if (resultPos != -1) {
        centerCursor();
    }
}

void
Editor::replaceCurrent()
{
    QString docText(document()->toPlainText());
    QTextCursor cursor = textCursor();
    int startBlock, endBlock, startPos, endPos;
    selectedBlocks(startBlock, endBlock, startPos, endPos);

    docText.replace(startPos, endPos - startPos,
        SearchReplaceDialog::get().replaceString());

    positionUpdateEnabled_ = false;

    QString name = ((Document*) document())->name();
    QString path = ((Document*) document())->path();

    document()->setPlainText(docText);

    ((Document*) document())->setName(name);
    ((Document*) document())->setPath(path);

    cursor.setPosition(endPos);
    setTextCursor(cursor);
    resetAllPositions();

    positionUpdateEnabled_ = true;

    search();
}

void
Editor::indent()
{
    int startBlock, endBlock, startPos, endPos;
    selectedBlocks(startBlock, endBlock, startPos, endPos);
    QTextCursor cursor = textCursor();

    // Store the value of the scrollbars.
    int scrollVal = verticalScrollBar()->value();

    // Add the indentation, starting from the back, moving towards the front.
    QString docText(document()->toPlainText());
    cursor.setPosition(endPos);
    int pos;
    int currBlock = cursor.blockNumber();
    cursor.movePosition(QTextCursor::StartOfBlock);
    while (currBlock >= startBlock) {
        pos = cursor.position();
        cursor.movePosition(QTextCursor::EndOfBlock);
        if (pos != cursor.position()) {
            cursor.movePosition(QTextCursor::StartOfBlock);
            if (isSpaceForTab()) {
                for (int num = 0; num < tabSize(); ++num) {
                    docText.insert(pos, ' ');
                    if (currBlock == startBlock && startBlock == endBlock) {
                        ++startPos;
                    }
                    ++endPos;
                }
            } else {
                docText.insert(pos, '\t');
                if (pos != startPos && currBlock == startBlock) {
                    ++startPos;
                }
                ++endPos;
            }
        }

        cursor.movePosition(QTextCursor::PreviousBlock);
        if (currBlock > 0) {
            currBlock = cursor.blockNumber();
        } else {
            currBlock = -1;
        }
    }

    positionUpdateEnabled_ = false;

    QString name = ((Document*) document())->name();
    QString path = ((Document*) document())->path();

    document()->setPlainText(docText);

    ((Document*) document())->setName(name);
    ((Document*) document())->setPath(path);

    // Restore the scrollbar positions.
    verticalScrollBar()->setValue(scrollVal);

    // Restore the selection.
    cursor.setPosition(startPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
    resetAllPositions(this);

    positionUpdateEnabled_ = true;
}

void
Editor::unindent()
{
    int startBlock, endBlock, startPos, endPos;
    selectedBlocks(startBlock, endBlock, startPos, endPos);
    QTextCursor cursor = textCursor();

    // Store the value of the scrollbars.
    int scrollVal = verticalScrollBar()->value();

    // Add the indentation, starting from the back, moving towards the front.
    QString docText(document()->toPlainText());
    cursor.setPosition(endPos);
    int pos;
    int currBlock = cursor.blockNumber();
    cursor.movePosition(QTextCursor::StartOfBlock);
    while (currBlock >= startBlock) {
        pos = cursor.position();
        if (docText.at(pos) == ' ') {
            for (int index = tabSize(); index > 0 && docText.at(pos) == ' ';
                    --index) {
                docText.remove(pos, 1);
                if (pos != startPos && currBlock == startBlock) {
                    --startPos;
                }
                if (pos != endPos || currBlock != startBlock) {
                    --endPos;
                }
            }
        } else if (docText.at(pos) == '\t') {
            docText.remove(pos, 1);
            if (pos != startPos && currBlock == startBlock) {
                --startPos;
            }
            if (pos != endPos || currBlock != startBlock) {
                --endPos;
            }
        }

        cursor.movePosition(QTextCursor::PreviousBlock);
        if (currBlock > 0) {
            currBlock = cursor.blockNumber();
        } else {
            currBlock = -1;
        }
    }

    positionUpdateEnabled_ = false;

    QString name = ((Document*) document())->name();
    QString path = ((Document*) document())->path();

    document()->setPlainText(docText);

    ((Document*) document())->setName(name);
    ((Document*) document())->setPath(path);

    // Restore the scrollbar positions.
    verticalScrollBar()->setValue(scrollVal);

    // Restore the selection.
    cursor.setPosition(startPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
    resetAllPositions(this);

    positionUpdateEnabled_ = true;
}

void
Editor::comment()
{
    QString commnt = SyntaxHighlighter::singleLineComments(
        ((Document*) document())->category());
    if (commnt.isEmpty()) {
        return;
    }

    int startBlock, endBlock, startPos, endPos;
    selectedBlocks(startBlock, endBlock, startPos, endPos);
    QTextCursor cursor = textCursor();

    // Store the value of the scrollbars.
    int scrollVal = verticalScrollBar()->value();

    // Move from back to front.
    QString docText(document()->toPlainText());
    cursor.setPosition(endPos);
    int pos;
    int currBlock = cursor.blockNumber();
    cursor.movePosition(QTextCursor::StartOfBlock);
    while (currBlock >= startBlock) {
        pos = cursor.position();
        if (docText.mid(pos, commnt.size()) == commnt) {
            // Line is commented. Uncomment.
            docText.remove(pos, commnt.size());
            if (currBlock == startBlock && pos != startPos) {
                startPos -= commnt.size();
            }
            if (pos != endPos) {
                endPos -= commnt.size();
            }
        } else {
            // Comment line.
            docText.insert(pos, commnt);
            if (currBlock == startBlock) {
                startPos += commnt.size();
            }
            endPos += commnt.size();
        }

        cursor.movePosition(QTextCursor::PreviousBlock);
        if (currBlock > 0) {
            currBlock = cursor.blockNumber();
        } else {
            currBlock = -1;
        }
    }

    positionUpdateEnabled_ = false;

    QString name = ((Document*) document())->name();
    QString path = ((Document*) document())->path();

    document()->setPlainText(docText);

    ((Document*) document())->setName(name);
    ((Document*) document())->setPath(path);

    // Restore the scrollbar positions.
    verticalScrollBar()->setValue(scrollVal);

    // Restore the selection.
    if (startPos < 0) {
        startPos = 0;
    }
    if (endPos < 0) {
        endPos = 0;
    }
    cursor.setPosition(startPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    setTextCursor(cursor);

    resetAllPositions(this);

    positionUpdateEnabled_ = true;
}

void
Editor::jumpToLine(int val)
{
    QTextCursor cursor(textCursor());

    if (val < 1) {
        cursor.movePosition(QTextCursor::Start);
        setTextCursor(cursor);
        return;
    }

    int currLine = cursor.blockNumber();
    if (val >= currLine) {
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,
            val - currLine);
    } else {
        cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor,
            currLine - val);
    }
    cursor.movePosition(QTextCursor::StartOfBlock);
    setTextCursor(cursor);
    centerCursor();
}

void
Editor::jumpToPosition(int val)
{
    QTextCursor cursor(textCursor());

    if (val < 0) {
        cursor.movePosition(QTextCursor::Start);
    } else if (val >= document()->characterCount()) {
        cursor.movePosition(QTextCursor::End);
    } else {
        cursor.setPosition(val);
    }
    setTextCursor(cursor);
    centerCursor();
}

void
Editor::setDocument(Document* doc)
{
    int index = 0;
    while (index < Document::count()) {
        if (Document::document(index) == doc) {
            break;
        }
        ++index;
    }

    if (index < Document::count()) {
        positionUpdateEnabled_ = false;
        QPlainTextEdit::setDocument(doc);
        setTabStopWidth(QFontMetrics(font_).width(" ") * tabSize_);
        update();
        if (statusBar_) {
            statusBar_->changeSelectedDocument(index);
        }
        selectAll();
        QTextCursor cursor(textCursor());
        cursor.setPosition(((Document*) document())->cursorPosition(this));
        setTextCursor(cursor);
        verticalScrollBar()->setValue(
            ((Document*) document())->scrollBarPosition(this));
        setFocus();
        updateCursorPosition();
        positionUpdateEnabled_ = true;
    }
}

void
Editor::setEditorFont(const QFont& font)
{
    static const QString char80String("012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789");
    int tabStopWidth = QFontMetrics(font).width(" ") * tabSize_;
    QFontMetrics fontMetrics(font);
    QVector<Editor*>::iterator iter = editors_.begin();

    font_ = font;
    char80Width_ = fontMetrics.width(char80String);

    while (iter != editors_.end()) {
        (*iter)->setFont(font_);
        (*iter)->setTabStopWidth(tabStopWidth);

        ++iter;
    }

    for (int index = 0; index < Document::count(); ++index) {
        Document::document(index)->setDefaultFont(font_);
    }
}

void
Editor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void
Editor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (!isLineNumberEnabled()) {
        return;
    }

    if (dy) {
        lineNumberArea_->scroll(0, dy);
    } else {
        lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(),
            rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void
Editor::updateCursorPosition()
{
    QTextCursor cursor = textCursor();
    if (positionUpdateEnabled_) {
        ((Document*) document())->setCursorPosition(this, cursor.position());
    }
    int col = cursor.columnNumber() + 1;
    int line = cursor.blockNumber() + 1;
    if (wordWrap()) {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::Left);
        if (cursor.blockNumber() + 1 == line) {
            col += cursor.columnNumber() + 1;
        }
    }
    if (statusBar_) {
        statusBar_->setLineNumber(line);
        statusBar_->setColumnNumber(col);
    }

    // Highlight the current line.
    if (currentLineColor_ != backgroundColor_) {
        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(currentLineColor_);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
        setExtraSelections(extraSelections);
    }
}

void
Editor::onScrollBarValueChanged(int value)
{
    if (positionUpdateEnabled_) {
        ((Document*) document())->setScrollBarPosition(this, value);
    }
}

void
Editor::onUpdateRequest(const QRect& rect, int dy)
{
    Q_UNUSED(rect);
    Q_UNUSED(dy);

    if (positionUpdateEnabled_ && Document::count() > 0) {
        ((Document*) document())->setScrollBarPosition(this,
            verticalScrollBar()->value());
    }
}

void
Editor::selectDocument(int index)
{
    if (index >= 0 && index < Document::count()) {
        positionUpdateEnabled_ = false;
        setDocument(Document::document(index));
        setTabStopWidth(QFontMetrics(font_).width(" ") * tabSize_);
        if (statusBar_) {
            statusBar_->changeSelectedDocument(index);
        }
        selectAll();
        QTextCursor cursor(textCursor());
        cursor.setPosition(((Document*) document())->cursorPosition(this));
        setTextCursor(cursor);
        verticalScrollBar()->setValue(
            ((Document*) document())->scrollBarPosition(this));
        setFocus();
        updateCursorPosition();
        positionUpdateEnabled_ = true;
    }
}

void
Editor::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Tab && isSpaceForTab()) {
        int size = tabSize() - (textCursor().columnNumber() % tabSize());
        for (int i = 0; i < size; ++i) {
            insertPlainText(" ");
        }
        event->accept();
    } else {
        QPlainTextEdit::keyPressEvent(event);
    }
}

void
Editor::resizeEvent(QResizeEvent* event)
{
    if (isLineNumberEnabled()) {
        QRect rectContents(contentsRect());
        lineNumberArea_->setGeometry(rectContents.left(), rectContents.top(),
            lineNumberAreaWidth(), rectContents.height());
    }

    QPlainTextEdit::resizeEvent(event);
}

bool
Editor::event(QEvent* event)
{
    if (event->type() == QEvent::FocusIn) {
        setActiveEditor(this);
    }

    return QPlainTextEdit::event(event);
}

void
Editor::paintEvent(QPaintEvent* event)
{
    if (char80Indicator_ != backgroundColor_) {
        QPainter painter(viewport());
        QRect rect(event->rect());
        rect.setLeft(char80Width_ + contentOffset().x() +
            document()->documentMargin());
        painter.fillRect(rect, char80Indicator_);
    }

    QPlainTextEdit::paintEvent(event);
}

void
Editor::selectedBlocks(
    int& startBlock,
    int& endBlock,
    int& startPos,
    int& endPos)
{
    QTextCursor cursor = textCursor();

    startPos = cursor.selectionStart();
    endPos = cursor.selectionEnd();
    cursor.setPosition(startPos);
    startBlock = cursor.blockNumber();
    cursor.setPosition(endPos);
    endBlock = cursor.blockNumber();

    int tmp;
    if (startBlock > endBlock) {
        tmp = startBlock;
        startBlock = endBlock;
        endBlock = tmp;
    }
    if (startPos > endPos) {
        tmp = startPos;
        startPos = endPos;
        endPos = tmp;
    }
}

void
Editor::resetAllPositions(Editor* excluded)
{
    Editor *edit;
    Document *doc;
    QTextCursor cursor;
    int index = 0;

    while (index < count()) {
        edit = editor(index);
        if (edit == excluded) {
            ++index;
            continue;
        }
        doc = (Document*) edit->document();
        cursor = edit->textCursor();
        cursor.setPosition(doc->cursorPosition(edit));
        edit->setTextCursor(cursor);
        edit->verticalScrollBar()->setValue(doc->scrollBarPosition(edit));
        ++index;
    }
}

#ifndef INLINE
#include "Editor.icc"
#endif // INLINE
