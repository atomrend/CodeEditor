/**
 *  @file SaveListDialog.h
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
 *  @brief When exiting the application or closing multiple documents, this
 *      dialog provides a list of documents that needs to be save with the
 *      capability to choose files to be saved. Files can only be saved to
 *      their stored file system path. This class is a singleton.
 */

#ifndef SAVE_LIST_DIALOG_H
#define SAVE_LIST_DIALOG_H

#include <QDialog>
class QTableWidget;

class SaveListDialog :
    public QDialog
{
    Q_OBJECT
public:
    /** Default constructor.
     *  @param[in] parent - Parent widget.
     */
    SaveListDialog(QWidget* parent = 0);

    /** Destructor.
     */
    ~SaveListDialog();

    static SaveListDialog& get(QWidget* parent = 0);

public slots:
    /** Activates and displays the dialog.
     *  @return A DialogCode indicating whether the dialog is accepted (1) or
     *      rejected (0).
     */
    int exec();

private slots:
    /** Callback that saves the selected documents.
     */
    void onSave();

private:
    QTableWidget *table_;

    static SaveListDialog *singleton_;
};

#endif // SAVE_LIST_DIALOG_H
