/**
 *  @file SyntaxHighlighter.h
 *
 *  @license: GNU Public License
 *      Copyright (c) 2012 Rory Strawther
 *      All Rights Reserved.
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Library General Public License for more details.
 *
 *      You should have received a copy of the GNU Library General Public
 *      License along with this library; if not, write to the
 *      Free Software Foundation, Inc.
 *      675 Mass Ave
 *      Cambridge, MA 02139
 *      U.S.A.
 *
 *  @author Rory Strawther
 *  @version 1.0
 *
 *  @brief Performs the syntax highlighting of the text and XML files.
 *      Comments, quotes, and keywords are highlighted in plain text files
 *      depending upon the category. XML nodes, comments, and quotes are
 *      highlighted in XML files.
 */

#ifndef SYNTAX_HIGHLIGHTER_H
#define SYNTAX_HIGHLIGHTER_H

#include <QColor>
#include <QHash>
#include <QStringList>
#include <QSyntaxHighlighter>
class Document;
class QColor;

class SyntaxHighlighter :
    public QSyntaxHighlighter
{
    Q_OBJECT
public:
    /** Default constructor.
     *  @param[in] doc - Document this syntax highlighter will be working upon.
     */
    SyntaxHighlighter(Document* doc);

    /** Get the keyword color.
     *  @return The keyword color.
     */
    static const QColor& keywordColor();

    /** Set the keyword color.
     *  @param[in] color - The new color to use for keywords.
     */
    static void setKeywordColor(const QColor& color);

    /** Get the comment color.
     *  @return The comment color.
     */
    static const QColor& commentColor();

    /** Set the comment color.
     *  @param[in] color - The new color to use for comments.
     */
    static void setCommentColor(const QColor& color);

    /** Get the quotation color.
     *  @return The quotation color.
     */
    static const QColor& quotationColor();

    /** Set the quotation color.
     *  @param[in] color - The new color to use for quotes.
     */
    static void setQuotationColor(const QColor& color);

    /** Get the list of categories.
     *  @return The list of categories of file types.
     */
    static QStringList categories();

    /** Get the list of keywords associated with the category.
     *  @param[in] category - The category to use to retrieve the keywords.
     *  @return The list of keywords associated with the category.
     */
    static const QStringList& keywords(const QString& category);

    /** Sets the list of keywords for the category.
     *  @param[in] category - The category to use to set the keywords.
     *  @param[in] list - The list of keywords to use for the category.
     */
    static void setKeywords(const QString& category, const QStringList& list);

    static void clearKeywords();

    /** Get the list of comments associated with the category.
     *  @param[in] category - The category to use to retrieve the comments.
     *  @return The list of comments associated with the category.
     */
    static const QString& singleLineComments(const QString& category);
    static const QString& multiLineStartComments(const QString& category);
    static const QString& multiLineEndComments(const QString& category);

    /** Sets the list of comments for the category.
     *  @param[in] category - The category to use to set the comments.
     *  @param[in] list - The list of comments to use for the category.
     */
    static void setSingleLineComments(
        const QString& category,
        const QString& comment);
    static void setMultiLineComments(
        const QString& category,
        const QString& start,
        const QString& end);

    static void clearComments();

protected:
    /** Highlights the given text block.
     *  @param[in] text - The text block to check for syntax highlighting.
     */
    virtual void highlightBlock(const QString& text);

    /** Highlights the given XML text block.
     *  @param[in] text - The XML text block to check for syntax highlighting.
     */
    void highlightXmlBlock(const QString& text);

private:
    /** The document this syntax highlighter works upon. */
    Document *document_;

    static QColor keywordColor_;
    static QColor commentColor_;
    static QColor quotationColor_;
    static QHash<QString, QStringList> keywords_;
    static QHash<QString, QString> singleLineComments_;
    static QHash<QString, QString> multiLineStartComments_;
    static QHash<QString, QString> multiLineEndComments_;
};

#endif // SYNTAX_HIGHLIGHTER_H
