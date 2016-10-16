/**
 *  @file PreferenceDialog.h
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
 *  @brief Provides a dialog to modify options not often changed.
 *      Some options in the dialog: font, colors, keywords, and key strokes.
 */

#ifndef PREFERENCE_DIALOG_H
#define PREFERENCE_DIALOG_H

#include <QColor>
#include <QDialog>
#include <QFont>
#include <QHash>
#include <QPixmap>
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;

class PreferenceDialog :
    public QDialog
{
    Q_OBJECT
public:
    PreferenceDialog(QWidget* parent = 0);
    ~PreferenceDialog();

    enum RemoteErrorCodes {
        Success = 0,
        Connection = 1,
        TimeOut = 2,
        Authentication = 3,
        FilePathTooLong = 4,
        CannotOpen = 5,
        BadFileSize = 6,
        BadFileChecksum = 7,
        FailedAction = 8,
        Port = 9,
        Address = 10,
        InvalidReceiveSize = 11
    };

    QString serverAddress() const;
    void setServerAddress(const QString& address);
    int serverPort() const;
    void setServerPort(const QString& port);
    QString serverPrivateKey() const;
    void setServerPrivateKey(const QString& key);
    QString serverPassword() const;
    QString serverAction() const;
    RemoteErrorCodes remoteErrorStatus() const;
    void setRemoteErrorStatus(RemoteErrorCodes status);

    static PreferenceDialog& get(QWidget* parent = 0);

public slots:
    int exec();

private slots:
    void onApply();
    void onFont();
    void onAddCategory();
    void onRemoveCategory();
    void onTextColor();
    void onKeywordColor();
    void onCommentColor();
    void onQuotationColor();
    void onBgColor();
    void onNondocColor();
    void onChar80Color();
    void onHiliteColor();
    void onCurrentLineColor();
    void onActiveLineNumTextColor();
    void onInactiveLineNumTextColor();
    void onActiveLineNumBgColor();
    void onInactiveLineNumBgColor();
    void onLineNumBorderColor();
    void onKeywordCategory(const QString& text);
    void onAddKeyword();
    void onRemoveKeyword();
    void onCommentCategory(const QString& text);
    void onSingleLineComment(const QString& text);
    void onMultilineStartComment(const QString& text);
    void onMultilineEndComment(const QString& text);
    void onExtensionCategory(const QString& text);
    void onAddExtension();
    void onRemoveExtension();

private:
    void setButtonColor(const QColor& color, QPushButton* button);
    void populateCategories();

private:
    // General.
    QFont font_;
    QPushButton *fontButton_;
    QCheckBox *lineNumberBorder_;
    QSpinBox *tabSize_;
    QListWidget *categories_;
    QLineEdit *newCategory_;
    QStringList categoryList_;

    // Color.
    QPushButton *textColorButton_;
    QPushButton *keywordColorButton_;
    QPushButton *commentColorButton_;
    QPushButton *quotationColorButton_;
    QPushButton *backgroundColorButton_;
    QPushButton *nondocumentColorButton_;
    QPushButton *char80ColorButton_;
    QPushButton *highlightColorButton_;
    QPushButton *currentLineColorButton_;
    QPushButton *activeLineNumberTextColorButton_;
    QPushButton *inactiveLineNumberTextColorButton_;
    QPushButton *activeLineNumberBackgroundColorButton_;
    QPushButton *inactiveLineNumberBackgroundColorButton_;
    QPushButton *lineNumberBorderColorButton_;
    QColor textColor_;
    QColor keywordColor_;
    QColor commentColor_;
    QColor quotationColor_;
    QColor backgroundColor_;
    QColor nondocumentColor_;
    QColor char80Color_;
    QColor highlightColor_;
    QColor currentLineColor_;
    QColor activeLineNumberTextColor_;
    QColor inactiveLineNumberTextColor_;
    QColor activeLineNumberBackgroundColor_;
    QColor inactiveLineNumberBackgroundColor_;
    QColor lineNumberBorderColor_;

    // Keyword.
    QComboBox *keywordCategory_;
    QListWidget *keywords_;
    QLineEdit *newKeyword_;
    QHash<QString, QStringList> keywordHash_;

    // Comment.
    QComboBox *commentCategory_;
    QLineEdit *singleLineComment_;
    QLineEdit *multiLineStartComment_;
    QLineEdit *multiLineEndComment_;
    QHash<QString, QString> singleLineCommentHash_;
    QHash<QString, QString> multiLineStartCommentHash_;
    QHash<QString, QString> multiLineEndCommentHash_;

    // Extension.
    QComboBox *extensionCategory_;
    QListWidget *extensions_;
    QLineEdit *newExtension_;
    QHash<QString, QStringList> extensionHash_;

    // Remote Server.
    QLineEdit *serverAddress_;
    QSpinBox *serverPort_;
    QLineEdit *serverPrivateKey_;
    QLineEdit *serverPassword_;
    QLineEdit *serverAction_;
    RemoteErrorCodes remoteErrorStatus_;
    QLabel *remoteStatus_;
    QLabel *remoteExplanation_;
    QPixmap remoteSuccess_;
    QPixmap remoteFailure_;

    static PreferenceDialog *preferenceDialog_;
};

#endif // PREFERENCE_DIALOG_H

