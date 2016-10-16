#include "SyntaxHighlighter.h"
#include "Document.h"
#include <QColor>
#include <QHash>

QColor SyntaxHighlighter::keywordColor_ = QColor(Qt::blue);
QColor SyntaxHighlighter::commentColor_ = QColor(191, 50, 255);
QColor SyntaxHighlighter::quotationColor_ = QColor(50, 191, 50);
QHash<QString, QStringList> SyntaxHighlighter::keywords_ =
    QHash<QString, QStringList>();
QHash<QString, QString> SyntaxHighlighter::singleLineComments_ =
    QHash<QString, QString>();
QHash<QString, QString> SyntaxHighlighter::multiLineEndComments_ =
    QHash<QString, QString>();
QHash<QString, QString> SyntaxHighlighter::multiLineStartComments_ =
    QHash<QString, QString>();

SyntaxHighlighter::SyntaxHighlighter(Document* doc) :
    QSyntaxHighlighter(doc),
    document_(doc)
{
}

const QColor&
SyntaxHighlighter::keywordColor()
{
    return keywordColor_;
}

void
SyntaxHighlighter::setKeywordColor(const QColor& color)
{
    keywordColor_ = color;
}

const QColor&
SyntaxHighlighter::commentColor()
{
    return commentColor_;
}

void
SyntaxHighlighter::setCommentColor(const QColor& color)
{
    commentColor_ = color;
}

const QColor&
SyntaxHighlighter::quotationColor()
{
    return quotationColor_;
}

void
SyntaxHighlighter::setQuotationColor(const QColor& color)
{
    quotationColor_ = color;
}

QStringList
SyntaxHighlighter::categories()
{
    return keywords_.keys();
}

const QStringList&
SyntaxHighlighter::keywords(const QString& category)
{
    return keywords_[category];
}

void
SyntaxHighlighter::setKeywords(const QString& category, const QStringList& list)
{
    keywords_[category] = list;
}

void
SyntaxHighlighter::clearKeywords()
{
    keywords_.clear();
}

const QString&
SyntaxHighlighter::singleLineComments(const QString& category)
{
    return singleLineComments_[category];
}

const QString&
SyntaxHighlighter::multiLineStartComments(const QString& category)
{
    return multiLineStartComments_[category];
}

const QString&
SyntaxHighlighter::multiLineEndComments(const QString& category)
{
    return multiLineEndComments_[category];
}

void
SyntaxHighlighter::setSingleLineComments(
    const QString& category,
    const QString& comment)
{
    singleLineComments_[category] = comment;
}

void
SyntaxHighlighter::setMultiLineComments(
    const QString& category,
    const QString& start,
    const QString& end)
{
    if (!start.isEmpty() && !end.isEmpty()) {
        multiLineStartComments_[category] = start;
        multiLineEndComments_[category] = end;
    }
}

void
SyntaxHighlighter::clearComments()
{
    singleLineComments_.clear();
    multiLineStartComments_.clear();
    multiLineEndComments_.clear();
}

int
numBackslashesBeforeQuote(const QString& text, int pos = 0)
{
    int result = 0;

    while (pos >= 0) {
        if (text.at(pos) == '\\') {
            ++result;
        } else {
            break;
        }
        --pos;
    }

    return result;
}

void
SyntaxHighlighter::highlightBlock(const QString& text)
{
    if (document_->category() == "XML") {
        highlightXmlBlock(text);
        return;
    }

    enum {
        Normal = -1,
        SingleQuote = 0,
        DoubleQuote = 1,
        Comment = 2
    };

    int top = 0;
    int bottom = 0;
    int currentIndex = 0;
    bool persist = true;
    int comparison = 0;
    QChar before;
    QChar after;
    QString currentValue;
    QString category = document_->category();
    QString singleLine = singleLineComments(category);
    QString multiStart = multiLineStartComments(category);
    QString multiEnd = multiLineEndComments(category);
    int state = previousBlockState();
    int start = 0;
    int i = 0;
    bool proceed = true;

    for (i = 0; i < text.length() && proceed; ++i) {
        if (state == Comment) {
            if (!multiEnd.isEmpty() &&
                    text.mid(i, multiEnd.size()) == multiEnd) {
                state = Normal;
                setFormat(start, i - start + multiEnd.size(), commentColor_);
            }
        } else if (state == DoubleQuote) {
            if (text.at(i) == '"' &&
                    numBackslashesBeforeQuote(text, i - 1) % 2 == 0) {
                state = Normal;
                setFormat(start, i - start + 1, quotationColor_);
            }
        } else if (state == SingleQuote) {
            if (text.at(i) == '\'' &&
                    numBackslashesBeforeQuote(text, i - 1) % 2 == 0) {
                state = Normal;
                setFormat(start, i - start + 1, quotationColor_);
            }
        } else {
            if (text.at(i) == '"') {
                // Double quotation.
                if (numBackslashesBeforeQuote(text, i - 1) % 2 == 0) {
                    start = i;
                    state = DoubleQuote;
                }
            } else if (text.at(i) == '\'') {
                // Single quotation.
                if (i == 0 ||
                        (numBackslashesBeforeQuote(text, i - 1) % 2 == 0 &&
                            !text.at(i - 1).isLetterOrNumber())) {
                    start = i;
                    state = SingleQuote;
                }
            } else {
                // Single line comments.
                if (!singleLine.isEmpty() &&
                        text.mid(i, singleLine.size()) == singleLine) {
                    setFormat(i, text.length() - i, commentColor_);
                    proceed = false;
                    break;
                }

                // Multiline comments.
                if (!multiStart.isEmpty() &&
                        text.mid(i, multiStart.size()) == multiStart) {
                    state = Comment;
                    start = i;
                }

                // Keywords.
                if (keywords_[category].size() > 0) {
                    bottom = 0;
                    top = keywords_[category].size() - 1;
                    currentIndex = top >> 1;
                    currentValue =
                        keywords_[category][currentIndex];
                    persist = true;
                } else {
                    persist = false;
                }

                while (persist) {
                    comparison = QString::compare(currentValue,
                        text.mid(i, currentValue.size()));
                    if (comparison == 0) {
                        if (i > 0) {
                            before = text.at(i - 1);
                        } else {
                            before = QChar();
                        }

                        if (i + currentValue.size() < text.size()) {
                            after = text.at(i + currentValue.size());
                        } else {
                            after = QChar();
                        }

                        if (!before.isLetterOrNumber() &&
                                !after.isLetterOrNumber() &&
                                before != '_' &&
                                after != '_') {
                            setFormat(i, currentValue.size(), keywordColor_);
                            persist = false;
                            i += currentValue.size() - 1;
                        }
                    }

                    if (top == bottom) {
                        persist = false;
                    } else {
                        if (top - bottom == 1 && currentIndex == top) {
                            top = bottom;
                        } else if (top - bottom == 1 &&
                                currentIndex == bottom) {
                            bottom = top;
                        } else if (comparison > 0) {
                            top = currentIndex - 1;
                        } else {
                            bottom = currentIndex + 1;
                        }
                        currentIndex = bottom + ((top - bottom) >> 1);
                        currentValue = keywords_[category][currentIndex];
                    }
                }
            }
        }
    }

    if (state == Comment) {
        setFormat(start, text.length() - start, commentColor_);
    } else if (state == DoubleQuote || state == SingleQuote) {
        setFormat(start, text.length() - start, quotationColor_);
    }

    setCurrentBlockState(state);
}

void
SyntaxHighlighter::highlightXmlBlock(const QString& text)
{
    enum {
        Normal = 0,
        SingleQuote = 1,
        DoubleQuote = 2,
        Comment = 3,
        Keyword = 4,
        InTag = 8
    };

    int state = previousBlockState();
    bool inTag = state & InTag;
    state &= ~InTag;
    QChar currChar;
    int start = 0;
    int i;

    for (i = 0; i < text.length(); ++i) {
        currChar = text.at(i);

        if (inTag) {
            if (currChar == '/') {
                setFormat(i, 1, keywordColor_);
            } else if (currChar == '>') {
                setFormat(i, 1, keywordColor_);
                inTag = false;
            }
        }
        if (state == Comment) {
            if (text.mid(i, 3) == "-->") {
                state = Normal;
                setFormat(start, i - start + 3, commentColor_);
            }
        } else if (state == DoubleQuote) {
            if (currChar == '"' &&
                    numBackslashesBeforeQuote(text, i - 1) % 2 == 0) {
                state = Normal;
                setFormat(start, i - start + 1, quotationColor_);
            }
        } else if (state == SingleQuote) {
            if (currChar == '\'' &&
                    numBackslashesBeforeQuote(text, i - 1) % 2 == 0) {
                state = Normal;
                setFormat(start, i - start + 1, quotationColor_);
            }
        } else if (state == Keyword) {
            if (currChar.isSpace() || currChar == '>') {
                state = Normal;
                setFormat(start, i - start, keywordColor_);
            }
        } else {
            if (currChar == '"') {
                // Double quotation.
                if (numBackslashesBeforeQuote(text, i - 1) % 2 == 0) {
                    start = i;
                    state = DoubleQuote;
                }
            } else if (text.at(i) == '\'') {
                // Single quotation.
                if (i == 0 ||
                        (numBackslashesBeforeQuote(text, i - 1) % 2 == 0 &&
                            !text.at(i - 1).isLetterOrNumber())) {
                    start = i;
                    state = SingleQuote;
                }
            } else {
                if (text.mid(i, 4) == "<!--") {
                    state = Comment;
                    start = i;
                } else if (currChar == '<') {
                    state = Keyword;
                    start = i;
                    inTag = true;
                }
            }
        }
    }

    if (state == Comment) {
        setFormat(start, text.length() - start, commentColor_);
    } else if (state == DoubleQuote || state == SingleQuote) {
        setFormat(start, text.length() - start, quotationColor_);
    } else if (state == Keyword) {
        setFormat(start, text.length() - start, keywordColor_);
        state = Normal;
    }

    if (inTag) {
        state |= InTag;
    }

    setCurrentBlockState(state);
}


