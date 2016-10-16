/**
 *  @file QuickOpenDialog.h
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
 *  @brief A simplified file dialog that is composed primarily of a line edit.
 *      Tab completion is supported that behaves similar to a terminal's
 *      tab completion. Glob expression matching is supported, as well.
 */

#ifndef QUICK_OPEN_DIALOG_H
#define QUICK_OPEN_DIALOG_H

#include <QDialog>
#include <QStringList>
class FileSystemLineEdit;

class QuickOpenDialog :
    public QDialog
{
    Q_OBJECT
protected:
    /** Default constructor.
     *  @param[in] parent - Parent widget.
     */
    QuickOpenDialog(QWidget* parent = NULL);

public:
    /** Gets the list of file paths specified by the user.
     *  @return The list of file paths.
     */
    QStringList paths() const;

    /** Get the common directory of the paths specified by the user.
     *  @return The directory last entered by the user.
     */
    QString directory() const;

    /** Sets the path that is displayed in the line edit.
     *  @param[in] path - The file system path to display.
     */
    void setCurrentPath(const QString& path);

    /** Retrieve the only instance (singleton) of this dialog.
     *  @return A reference to the dialog.
     */
    static QuickOpenDialog& get(QWidget* parent = NULL);

public slots:
    /** Activates and displays the dialog.
     *  @return A DialogCode indicating whether the dialog is accepted (1) or
     *      rejected (0).
     */
    int exec();

protected:
    /** The line edit in which the user types the file path. */
    FileSystemLineEdit *edit_;

    /** Stores the singleton pointer for this dialog. */
    static QuickOpenDialog* dialog_;
};

#endif // QUICK_OPEN_DIALOG_H
