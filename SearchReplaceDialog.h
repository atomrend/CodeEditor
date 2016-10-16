/**
 *  @file SearchReplaceDialog.h
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
 *  @brief Provides a dialog to initiate a search or a replace action for the
 *      active editor. Provides a preview pane to view results that match the
 *      search term.
 */

#ifndef SEARCH_REPLACE_DIALOG_H
#define SEARCH_REPLACE_DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QString>
#include <QVector>
class PreviewLineNumberArea;
class PreviewSyntaxHighlighter;
class PreviewTextEdit;
class QLineEdit;
class QPaintEvent;
class QPushButton;
class QRadioButton;
class QRect;
class QResizeEvent;

class SearchReplaceDialog :
    public QDialog
{
    Q_OBJECT
public:
    /** Default constructor.
     *  @param[in] parent - Parent widget.
     */
    SearchReplaceDialog(QWidget* parent = 0);

    /** Destructor.
     */
    ~SearchReplaceDialog();

    QString searchString();
    QString replaceString();
    bool searchCaseSensitive();
    bool searchDirectionForward();

    static SearchReplaceDialog& get();

public slots:
    int exec();

protected slots:
    void preview();
    void replaceAll();

private:
    QLineEdit *searchLineEdit_;
    QLineEdit *replaceLineEdit_;
    QRadioButton *caseSensitivityButton_;
    QRadioButton *directionButton_;
    PreviewTextEdit *preview_;

    /** Stores the singleton pointer for this dialog. */
    static SearchReplaceDialog* searchReplaceDialog_;
};

class PreviewTextEdit :
    public QPlainTextEdit
{
    Q_OBJECT
public:
    PreviewTextEdit(QWidget* parent = 0);
    ~PreviewTextEdit();

    void clear();

    void addResult(int lineNum, const QString& line);

    void setSearchString(const QString& string);

    void lineNumberAreaPaintEvent(QPaintEvent* event);

    int lineNumberAreaWidth();

    void rehighlightSyntax();

private slots:
    void updateLineNumberAreaWidth(int);
    void updateLineNumberArea(const QRect&, int);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    QVector<int> resultLineNumbers_;
    PreviewLineNumberArea *lineNumberArea_;
    PreviewSyntaxHighlighter *syntax_;
};

#endif // SEARCH_REPLACE_DIALOG_H
