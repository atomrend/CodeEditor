/**
 *  @file ActionEditorDialog.h
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
 *  @brief This dialog window provides the capability to change parameters of
 *      QActions. The parameters of a QAction that can be changed are the text,
 *      the descriptive text, and the shortcut sequence.
 */

#ifndef ACTION_EDITOR_H
#define ACTION_EDITOR_H

#include <QString>
#include <QWidget>
#include <QVector>
class QAction;
class QLabel;
class QTableWidget;
class QTableWidgetItem;

class ActionEditor :
    public QWidget
{
    Q_OBJECT
public:
    /** Default constructor.
     *  @param[in] parent - Parent widget.
     */
    ActionEditor(QWidget* parent = 0);

    /** Destructor.
     */
    ~ActionEditor();

    void setup();

    /** Add an action that can be modified through this dialog.
     *  @param[in] act - The QAction to add to this dialog.
     */
    static void addAction(QAction* action);

    /** Removes an action from the dialog.
     *  @param[in] act - The QAction to remove.
     *  @return Boolean indicating whether the QAction was found and removed.
     */
    static bool removeAction(QAction* action);

    static int count();

    static QAction* action(int index);

    static ActionEditor* get();

private slots:
    /** Validates the text entered for a QAction. Validates only the text and
     *      shortcut fields. The shortcut must be a valid shortcut key sequence.
     *      The text and shortcut must not duplicate that of another QAction.
     *  @param[in] item - The table cell to validate.
     */
    void validate(QTableWidgetItem* item);

    /** Saves the text for the table cell being edited in case an invalid value
     *      is inputted and needs to be replaced with the original
     *  @param[in] item - The table cell to save.
     */
    void saveOldValue(QTableWidgetItem* item);

private:
    QVector<QAction*> actions_;
    QLabel *statusLabel_;
    QTableWidget *table_;
    QString previousValue_;

    static ActionEditor *singleton_;
};

#endif // ACTION_EDITOR_H
