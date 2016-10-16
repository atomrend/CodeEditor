/**
 *  @file Document.h
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
 *  @brief This is the container for an open file. File system operations,
 *      such as opening and saving, are handled by this class.
 */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QHash>
#include <QStringList>
#include <QTextDocument>
#include <QVector>
class Editor;
class QThread;
class RemoteSaveWorker;
class SyntaxHighlighter;

class Document :
    public QTextDocument
{
    Q_OBJECT
public:
    /** Default constructor.
     *  @param[in] name - The name of the document.
     *  @param[in] path - The file system path of the document.
     */
    Document(const QString& path);

    /** Destructor.
     */
    ~Document();

    /** Opens the document using the file system path. Useful for initially
     *      opening the file or reverting the document to the file.
     *  @return Boolean indicating whether the file could be read into this
     *      document.
     */
    bool open();

    /** Saves the document.
     *  @return Boolean indicating whether the save was successful.
     */
    bool save();

    /** Saves the document to a new path. If the save is successful,
     *      both the name and the path will be changed.
     *  @param[in] path - The new file system path to save to.
     *  @return Boolean indicating whether the save was successful.
     */
    bool saveAs(const QString& path);

    /** Indicates whether the document has been modified and needs to be saved.
     *  @return Boolean indicating whether the document needs to be saved.
     */
    bool needToSave() const;

    /** Get the name of the document.
     *  @return The document's name.
     */
    QString name() const;

    /** Set the document name.
     *  @param[in] name - The new name to assign to the document.
     */
    void setName(const QString& name);

    /** Get the file system path.
     *  @return The file system path to save this document to.
     */
    QString path() const;

    /** Set the file system path where the document will be saved to.
     *  @param[in] path - The new file system path to use.
     */
    void setPath(const QString& path);

    /** Get the category associated with this document. The category is used
     *      for syntax highlighting.
     *  @return The category of this document.
     */
    const QString& category() const;

    /** Set the category of this document. The category is used for syntax
     *      highlighting.
     *  @param[in] category - The category to use for this document.
     */
    void setCategory(const QString& category);

    /** Get the cursor position for the specified editor.
     *  @param[in] editor - The index of the editor for which to get the
     *      position.
     *  @return The cursor position stored for the editor.
     */
    int cursorPosition(Editor* editor) const;

    /** Store the cursor position for a specified editor.
     *  @param[in] editor - The editor index associated with the cursor
     *      position.
     *  @param[in] pos - The cursor position.
     */
    void setCursorPosition(Editor* editor, const int pos);

    /** Get the vertical scroll bar position of this document for a specified
     *      editor.
     *  @param[in] editor - The index of the editor for which to get the scroll
     *      bar position.
     *  @return The vertical scroll bar position of this document
     *      for the editor.
     */
    int scrollBarPosition(Editor* editor) const;

    /** Store the vertical scroll bar position of this document for a specified
     *      editor.
     *  @param[in] editor - The index associated with the scroll bar position.
     *  @param[in] pos - The vertical scroll bar position.
     */
    void setScrollBarPosition(Editor* editor, const int pos);

    /** Causes the syntax highlighter to reapply the syntax highlighting.
     *      This should be done after a change to the keyword or comment list
     *      or syntax color.
     */
    void rehighlightSyntax();

    /** Get the file path the document is saved to on the remote server.
     *  @return The remote file path.
     */
    const QString& remotePath() const;

    /** Set the remote file path used to save this document on the remote
     *      server.
     *  @param[in] remotePath - The file path on the remote server.
     */
    void setRemotePath(const QString& remotePath);

    /** Get the status of the last remote save attempt.
     *      A failure indicates the remote server was not able to save because
     *      the remote file path is invalid or the server did not have
     *      permission to write to the path.
     *  @return A boolean to indicate the last save status. True = successful.
     *      False = failure.
     */
    bool remoteSaveSuccessful() const;

    /** Set whether the last remote save attempt was successful or not.
     *      A failure indicates the remote server was not able to save because
     *      the remote file path is invalid or the server did not have
     *      permission to write to the path.
     *  @param[in] status - The status of the last remote save attempt.
     */
    void setRemoveSaveSuccessful(bool status);

    /** Causes the syntax highter to apply syntax highlighting to all documents.
     *      This should be done after a change to the keyword or comment list
     *      or syntax color.
     */
    static void rehighlightSyntaxAllDocuments();

    static const QStringList& extensions(const QString& category);

    static void setExtensions(
        const QString& category,
        const QStringList& extensions);

    static void clearExtensions();

    /** The number of documents available.
     *  @return The number of documents available.
     */
    static int count();

    /** Get a specified document.
     *  @param[in] index - The index of the document.
     *  @return The particular document assigned to this index.
     *      NULL if an invalid index is specified.
     */
    static Document* document(int index);

    static void sortDocumentList();

protected:
    void determineCategory();
    int findMatchingName(const QString& name) const;
    void determineName();

private slots:
    /** Callback for when the document is modified.
     *      Used to indicate whether the document needs to be saved.
     */
    void modified();

private:
    /** Category of the document. This is used for the syntax highlighting. */
    QString category_;

    /** Indicates whether the document has been modified and
     *      needs to be saved. */
    bool needToSave_;

    /** The syntax highlighter associated with this document. */
    SyntaxHighlighter *syntaxHighlighter_;

    /** The cursor position of this document for a specific editor. */
    QHash<Editor*, int> cursorPosition_;

    /** The scroll bar position of this document for a specific editor. */
    QHash<Editor*, int> scrollBarPosition_;

    /** File path to save the document on the remote server. */
    QString remotePath_;

    /** Indicates whether the remote save attemp was successful. */
    bool remoteSaveSuccessful_;

    /** Thread use to save the document to the remote server in the background. */
    QThread *thread_;

    /** Worker class to connect to the remote server and save this document. */
    RemoteSaveWorker *remoteSaveWorker_;

    /** List of file extensions associated with a category. */
    static QHash<QString, QStringList> extensions_;

    /** List of all available documents. */
    static QVector<Document*> documents_;
};

#ifdef INLINE
#include "Document.icc"
#endif // INLINE

#endif // DOCUMENT_H
