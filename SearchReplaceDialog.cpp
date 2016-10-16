#include "SearchReplaceDialog.h"
#include "Editor.h"
#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFontMetrics>
#include <QGridLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QSyntaxHighlighter>
#include <QTextDocument>

// #############################################################################
// PreviewSyntaxHighlighter
// #############################################################################

class PreviewSyntaxHighlighter :
    public QSyntaxHighlighter
{
public:
    PreviewSyntaxHighlighter(QTextDocument* parent = 0);

    void setSearchString(const QString& string);

protected:
    virtual void highlightBlock(const QString& text);

private:
    QString searchString_;
};

PreviewSyntaxHighlighter::PreviewSyntaxHighlighter(QTextDocument* parent) :
    QSyntaxHighlighter(parent),
    searchString_(QString())
{
}

void
PreviewSyntaxHighlighter::setSearchString(const QString& string)
{
    searchString_ = string;
}

void
PreviewSyntaxHighlighter::highlightBlock(const QString& text)
{
    static QTextCharFormat format;
    static bool firstTime = true;
    if (firstTime) {
        format.setBackground(QBrush(QColor(255, 255, 151)));
        firstTime = false;
    }
    int index = -1;
    while ((index = text.indexOf(searchString_, index + 1)) != -1) {
        setFormat(index, searchString_.size(), format);
    }
}

// #############################################################################
// PreviewLineNumberArea
// #############################################################################

class PreviewLineNumberArea :
    public QWidget
{
public:
    PreviewLineNumberArea(PreviewTextEdit* parent = 0);
    ~PreviewLineNumberArea();

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event);

private:
    PreviewTextEdit *editor_;
};

PreviewLineNumberArea::PreviewLineNumberArea(PreviewTextEdit* parent) :
    QWidget(parent),
    editor_(parent)
{
}

PreviewLineNumberArea::~PreviewLineNumberArea()
{
}

QSize
PreviewLineNumberArea::sizeHint() const
{
    return QSize(editor_->lineNumberAreaWidth(), 0);
}

void
PreviewLineNumberArea::paintEvent(QPaintEvent* event)
{
    editor_->lineNumberAreaPaintEvent(event);
}

// #############################################################################
// SearchReplaceDialog
// #############################################################################

SearchReplaceDialog* SearchReplaceDialog::searchReplaceDialog_ = NULL;

SearchReplaceDialog::SearchReplaceDialog(QWidget* parent) :
    QDialog(parent),
    searchLineEdit_(new QLineEdit),
    replaceLineEdit_(new QLineEdit),
    caseSensitivityButton_(new QRadioButton(tr("Sensitive"))),
    directionButton_(new QRadioButton(tr("Forward"))),
    preview_(new PreviewTextEdit)
{
    if (searchReplaceDialog_ == NULL) {
        searchReplaceDialog_ = this;
    }

    setWindowTitle(tr("Search and Replace"));

    QGridLayout *layout = new QGridLayout;
    setLayout(layout);

    // Search line edit.
    QLabel *searchLabel = new QLabel(tr("Search: "));
    layout->addWidget(searchLabel, 0, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(searchLineEdit_, 0, 1, 1, 3);

    // Replace line edit.
    QLabel *replaceLabel = new QLabel(tr("Replace: "));
    layout->addWidget(replaceLabel, 1, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(replaceLineEdit_, 1, 1, 1, 3);

    // Case Sensitivity.
    QLabel *sensitiveLabel = new QLabel(tr("Case Sensitivity: "));
    layout->addWidget(sensitiveLabel, 2, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(caseSensitivityButton_, 2, 1);
    QRadioButton *insensitiveButton = new QRadioButton(tr("Insensitive"));
    layout->addWidget(insensitiveButton, 2, 2);
    QButtonGroup *sensitiveGroup = new QButtonGroup;
    sensitiveGroup->addButton(caseSensitivityButton_);
    sensitiveGroup->addButton(insensitiveButton);
    caseSensitivityButton_->setChecked(true);
    insensitiveButton->setChecked(false);

    // Search direction.
    QLabel *directionLabel = new QLabel(tr("Direction: "));
    layout->addWidget(directionLabel, 3, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(directionButton_, 3, 1);
    QRadioButton *backwardButton = new QRadioButton(tr("Backward"));
    layout->addWidget(backwardButton, 3, 2);
    QButtonGroup *directionGroup = new QButtonGroup;
    directionGroup->addButton(directionButton_);
    directionGroup->addButton(backwardButton);
    directionButton_->setChecked(true);
    backwardButton->setChecked(false);
    
    // Buttons.
    QPushButton *button;

    button = new QPushButton(tr("&Search"));
    button->setDefault(true);
    layout->addWidget(button, 5, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    button = new QPushButton(tr("&Replace All"));
    layout->addWidget(button, 5, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(replaceAll()));

    button = new QPushButton(tr("&Preview"));
    layout->addWidget(button, 5, 2);
    connect(button, SIGNAL(clicked()), this, SLOT(preview()));

    button = new QPushButton(tr("Close"));
    layout->addWidget(button, 5, 3);
    connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    QAction *action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    connect(action, SIGNAL(triggered()), this, SLOT(preview()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(action, SIGNAL(triggered()), this, SLOT(replaceAll()));
    addAction(action);

    // Preview pane.
    layout->addWidget(preview_, 6, 0, 1, 4);
    QFontMetrics metrics(Editor::editorFont());
    preview_->setFixedWidth(metrics.width("012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789") + 50);
}

SearchReplaceDialog::~SearchReplaceDialog()
{
}

QString
SearchReplaceDialog::searchString()
{
    return searchReplaceDialog_->searchLineEdit_->text();
}

QString
SearchReplaceDialog::replaceString()
{
    return searchReplaceDialog_->replaceLineEdit_->text();
}

bool
SearchReplaceDialog::searchCaseSensitive()
{
    return searchReplaceDialog_->caseSensitivityButton_->isChecked();
}

bool
SearchReplaceDialog::searchDirectionForward()
{
    return searchReplaceDialog_->directionButton_->isChecked();
}

SearchReplaceDialog&
SearchReplaceDialog::get()
{
    if (searchReplaceDialog_ == NULL) {
        searchReplaceDialog_ = new SearchReplaceDialog;
    }

    return *searchReplaceDialog_;
}

int
SearchReplaceDialog::exec()
{
    replaceLineEdit_->clear();
    searchLineEdit_->selectAll();
    searchLineEdit_->setFocus();

    return QDialog::exec();
}

void
SearchReplaceDialog::preview()
{
    QTextDocument::FindFlag findFlag = (QTextDocument::FindFlag) 0;
    if (searchCaseSensitive()) {
        findFlag = QTextDocument::FindCaseSensitively;
    }

    preview_->clear();
    preview_->setSearchString(searchString());
    QTextDocument *doc = Editor::activeEditor()->document();
    int lastLineNum = -1;
    QTextCursor cursor = doc->find(searchString(), 0, findFlag);
    while (!cursor.isNull()) {
        if (lastLineNum != cursor.blockNumber() + 1) {
            lastLineNum = cursor.blockNumber() + 1;
            preview_->addResult(lastLineNum, cursor.block().text());
        }
        cursor = doc->find(searchString(), cursor.position() + 1,
            findFlag);
    }
    preview_->rehighlightSyntax();
}

void
SearchReplaceDialog::replaceAll()
{
    QTextDocument *document = Editor::activeEditor()->document();
    QString documentText = document->toPlainText();
    Qt::CaseSensitivity caseSensitive = Qt::CaseSensitive;
    if (!searchCaseSensitive()) {
        caseSensitive = Qt::CaseInsensitive;
    }
    int index = documentText.indexOf(searchString(), 0, caseSensitive);
    while (index != -1) {
        documentText.replace(index, searchString().size(), replaceString());
        index += replaceString().size();
        index = documentText.indexOf(searchString(), index, caseSensitive);
    }

    QString name = ((Document*) document)->name();
    QString path = ((Document*) document)->path();

    document->setPlainText(documentText);

    ((Document*) document)->setName(name);
    ((Document*) document)->setPath(path);
}

// #############################################################################
// PreviewTextEdit
// #############################################################################

PreviewTextEdit::PreviewTextEdit(QWidget* parent) :
    QPlainTextEdit(parent),
    resultLineNumbers_(QVector<int>()),
    lineNumberArea_(new PreviewLineNumberArea(this)),
    syntax_(new PreviewSyntaxHighlighter(document()))
{
    setReadOnly(true);
    setBackgroundVisible(true);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    updateLineNumberAreaWidth(0);

    connect(this, SIGNAL(blockCountChanged(int)), this,
        SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(const QRect&, int)), this,
        SLOT(updateLineNumberArea(const QRect&, int)));
}

PreviewTextEdit::~PreviewTextEdit()
{
    delete lineNumberArea_;
    delete syntax_;
}

void
PreviewTextEdit::clear()
{
    QPlainTextEdit::clear();
    resultLineNumbers_.clear();
}

void
PreviewTextEdit::addResult(int lineNum, const QString& line)
{
    resultLineNumbers_.append(lineNum);
    appendPlainText(line);
}

void
PreviewTextEdit::setSearchString(const QString& string)
{
    syntax_->setSearchString(string);
}

void
PreviewTextEdit::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea_);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top =
        (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    QRect rect(event->rect());

    painter.fillRect(rect, Qt::lightGray);

    while (block.isValid() && top <= rect.bottom()) {
        if (block.isVisible() && bottom >= rect.top()) {
            painter.drawText(-2, top, lineNumberArea_->width(),
                fontMetrics().height(), Qt::AlignRight,
                QString::number(resultLineNumbers_.at(blockNumber)));
        }
        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int 
PreviewTextEdit::lineNumberAreaWidth()
{
    if (resultLineNumbers_.isEmpty()) {
        return 0;
    }

    int digits(0);
    int value = resultLineNumbers_.back();
    while (value) {
        value /= 10;
        ++digits;
    }

    QFontMetrics fontMetrics(font());
    
    return fontMetrics.width(QLatin1Char('9')) * digits + 3;
}

void
PreviewTextEdit::rehighlightSyntax()
{
    syntax_->rehighlight();
}

void
PreviewTextEdit::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void
PreviewTextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
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
PreviewTextEdit::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(),
        lineNumberAreaWidth(), cr.height()));
}


