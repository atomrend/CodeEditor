#include "PreferenceFile.h"
#include "ActionEditor.h"
#include "Document.h"
#include "Editor.h"
#include "PreferenceDialog.h"
#include <QAction>
#include <QColor>
#include <QFile>
#include <QHash>
#include <QPoint>
#include <QSize>
#include <QStringList>
#include <QtXml>
#include "SyntaxHighlighter.h"

QPoint position_ = QPoint(0, 0);
QSize resolution_ = QSize(0, 0);

int
readPreferenceFile(const QString& filePath)
{
    int result = 0;
    bool statusBar = true;
    bool menuBar = true;
    bool spaceForTab = true;
    bool lineNumberBorder = true;
    bool lineNumber = true;
    bool wordWrap = false;
    int tabSize = 4;
    QFont font("Monospace", 10);
    QColor textColor(Qt::black);
    QColor backgroundColor(Qt::white);
    QColor nondocumentColor(127, 127, 127);
    QColor char80Indicator = QColor(Qt::white).darker(107);
    QColor highlightColor = QColor(0, 0, 191);
    QColor currentLineColor = QColor(Qt::yellow).lighter(160);
    QColor activeLineNumberForegroundColor(Qt::black);
    QColor inactiveLineNumberForegroundColor(Qt::black);
    QColor activeLineNumberBackgroundColor(191, 191, 191);
    QColor inactiveLineNumberBackgroundColor(Qt::white);
    QColor lineNumberBorderColor(Qt::black);
    QColor keywordColor(Qt::blue);
    QColor commentColor(191, 50, 255);
    QColor quotationColor(50, 191, 50);
    QStringList stringList;
    QList<QString>::const_iterator iter;
    QString category = "Default";
    QHash<QString, QStringList> keywordHash;
    QHash<QString, QStringList> extensionHash;
    QHash<QString, QString> singleLineCommentHash;
    QHash<QString, QString> multiLineStartCommentHash;
    QHash<QString, QString> multiLineEndCommentHash;
    QHash<QString, QString> actionMenuNameHash;
    QHash<QString, QString> actionShortcutHash;
    QHash<QString, QString> actionDescriptionHash;
    QString string;
    QString elementName;
    QXmlStreamReader reader;
    QXmlStreamAttributes attributes;
    QAction *action;
    int index;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        reader.setDevice(&file);

        while (!reader.atEnd()) {
            reader.readNext();
            elementName = reader.name().toString();

            if (elementName == "codedit_prefs") {
                // Do nothing.
            } else if (elementName == "PositionSize" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                if (!attributes.value(QString(), "max").isEmpty()) {
                    result = 1;
                } else if (!attributes.value(QString(), "full").isEmpty()) {
                    result = 2;
                } else {
                    string = attributes.value(QString(), "x").toString();
                    if (!string.isEmpty()) {
                        position_.setX(string.toInt());
                    }

                    string = attributes.value(QString(), "y").toString();
                    if (!string.isEmpty()) {
                        position_.setY(string.toInt());
                    }

                    string = attributes.value(QString(), "width").toString();
                    if (!string.isEmpty()) {
                        resolution_.setWidth(string.toInt());
                    }

                    string = attributes.value(QString(), "height").toString();
                    if (!string.isEmpty()) {
                        resolution_.setHeight(string.toInt());
                    }
                }
            } else if (elementName == "Font" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                string = attributes.value(QString(), "family").toString();
                if (!string.isEmpty()) {
                    font.setFamily(string);
                }
                string = attributes.value(QString(), "size").toString();
                if (!string.isEmpty()) {
                    font.setPointSize(string.toInt());
                }
            } else if (elementName == "TabSize" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                string = attributes.value(QString(), "size").toString();
                if (!string.isEmpty()) {
                    tabSize = string.toInt();
                }
            } else if (elementName == "WordWrap" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                if (attributes.value(QString(), "enabled").toString() == "1") {
                    wordWrap = true;
                } else {
                    wordWrap = false;
                }
            } else if (elementName == "StatusBar" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                if (attributes.value(QString(), "enabled").toString() == "1") {
                    statusBar = true;
                } else {
                    statusBar = false;
                }
            } else if (elementName == "SpaceForTab" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                if (attributes.value(QString(), "enabled").toString() == "1") {
                    spaceForTab = true;
                } else {
                    spaceForTab = false;
                }
            } else if (elementName == "MenuBar" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                if (attributes.value(QString(), "enabled").toString() == "1") {
                    menuBar = true;
                } else {
                    menuBar = false;
                }
            } else if (elementName == "LineNumber" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                if (attributes.value(QString(), "enabled").toString() == "1") {
                    lineNumber = true;
                } else {
                    lineNumber = false;
                }
            } else if (elementName == "LineNumberBorder" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                if (attributes.value(QString(), "enabled").toString() == "1") {
                    lineNumberBorder = true;
                } else {
                    lineNumberBorder = false;
                }
            } else if (elementName == "TextColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                textColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "KeywordColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                keywordColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "CommentColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                commentColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "QuotationColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                quotationColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "BackgroundColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                backgroundColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "HighlightColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                highlightColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "CurrentLineColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                currentLineColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "NondocumentColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                nondocumentColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "Character80Color" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                char80Indicator.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "ActiveLineNumberTextColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                activeLineNumberForegroundColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "InactiveLineNumberTextColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                inactiveLineNumberForegroundColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "ActiveLineNumberBackgroundColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                activeLineNumberBackgroundColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "InactiveLineNumberBackgroundColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                inactiveLineNumberBackgroundColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "LineNumberBorderColor" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                lineNumberBorderColor.setNamedColor(
                    attributes.value(QString(), "color").toString());
            } else if (elementName == "Category" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                category = attributes.value(QString(), "cat").toString();
            } else if (elementName == "Keywords" && reader.isStartElement()) {
                stringList.clear();
            } else if (elementName == "Keywords" && reader.isEndElement()) {
                keywordHash[category] = stringList;
            } else if (elementName == "Keyword" && reader.isStartElement()) {
                reader.readNext();
                stringList.append(reader.text().toString());
            } else if (elementName == "Extensions" && reader.isStartElement()) {
                stringList.clear();
            } else if (elementName == "Extensions" && reader.isEndElement()) {
                extensionHash[category] = stringList;
            } else if (elementName == "Extension" && reader.isStartElement()) {
                reader.readNext();
                stringList.append(reader.text().toString());
            } else if (elementName == "SingleLineComment" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                singleLineCommentHash[category] =
                    attributes.value(QString(), "text").toString();
            } else if (elementName == "MultiLineStartComment" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                multiLineStartCommentHash[category] =
                    attributes.value(QString(), "text").toString();
            } else if (elementName == "MultiLineEndComment" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                multiLineEndCommentHash[category] =
                    attributes.value(QString(), "text").toString();
            } else if (elementName == "Action" && reader.isStartElement()) {
                attributes = reader.attributes();
                category = attributes.value(QString(), "ID").toString();
            } else if (elementName == "MenuName" && reader.isStartElement()) {
                reader.readNext();
                actionMenuNameHash[category] = reader.text().toString();
            } else if (elementName == "Shortcut" && reader.isStartElement()) {
                reader.readNext();
                actionShortcutHash[category] = reader.text().toString();
            } else if (elementName == "Description" &&
                    reader.isStartElement()) {
                reader.readNext();
                actionDescriptionHash[category] = reader.text().toString();
            } else if (elementName == "Address" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                string = attributes.value(QString(), "text").toString();
                PreferenceDialog::get().setServerAddress(
                    string);
            } else if (elementName == "Port" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                string = attributes.value(QString(), "text").toString();
                PreferenceDialog::get().setServerPort(string);
            } else if (elementName == "PrivateKey" &&
                    reader.isStartElement()) {
                attributes = reader.attributes();
                string = attributes.value(QString(), "text").toString();
                PreferenceDialog::get().setServerPrivateKey(
                    string);
            }
        }

        file.close();
    }

    if (!keywordHash.contains("Default")) {
        category = "Default";
        stringList = QStringList();
        Document::setExtensions(category, stringList);
        SyntaxHighlighter::setKeywords(category, stringList);
        SyntaxHighlighter::setSingleLineComments(category, QString("#"));
        SyntaxHighlighter::setMultiLineComments(category, QString(), QString());
    }

    MainWindow::get().setMenuBarVisible(menuBar);
    MainWindow::get().setStatusBarVisible(statusBar);
    Editor::setForegroundColor(textColor);
    Editor::setBackgroundColor(backgroundColor);
    Editor::setNondocumentColor(nondocumentColor);
    Editor::setChar80IndicatorColor(char80Indicator);
    Editor::setHighlightColor(highlightColor);
    Editor::setCurrentLineColor(currentLineColor);
    Editor::setActiveLineNumberForegroundColor(activeLineNumberForegroundColor);
    Editor::setInactiveLineNumberForegroundColor(
        inactiveLineNumberForegroundColor);
    Editor::setActiveLineNumberBackgroundColor(activeLineNumberBackgroundColor);
    Editor::setInactiveLineNumberBackgroundColor(
        inactiveLineNumberBackgroundColor);
    Editor::setLineNumberBorderColor(lineNumberBorderColor);
    Editor::setLineNumberBorderEnabled(lineNumberBorder);
    Editor::setLineNumberEnabled(lineNumber);
    Editor::setSpaceForTab(spaceForTab);
    Editor::setWordWrap(wordWrap);
    Editor::setTabSize(tabSize);
    Editor::setEditorFont(font);
    MainWindow::get().updateActionCheckMarks();
    SyntaxHighlighter::setKeywordColor(keywordColor);
    SyntaxHighlighter::setCommentColor(commentColor);
    SyntaxHighlighter::setQuotationColor(quotationColor);

    stringList = keywordHash.keys();
    iter = stringList.constBegin();
    while (iter != stringList.constEnd()) {
        SyntaxHighlighter::setKeywords(*iter, keywordHash[*iter]);
        SyntaxHighlighter::setSingleLineComments(*iter,
            singleLineCommentHash[*iter]);
        SyntaxHighlighter::setMultiLineComments(*iter,
            multiLineStartCommentHash[*iter],
            multiLineEndCommentHash[*iter]);
        Document::setExtensions(*iter, extensionHash[*iter]);

        ++iter;
    }

    index = 0;
    while (index < ActionEditor::count()) {
        action = ActionEditor::action(index);
        category = action->data().toString();
        if (actionMenuNameHash.contains(category)) {
            action->setText(actionMenuNameHash[category]);
            action->setShortcut(
                QKeySequence::fromString(actionShortcutHash[category]));
            action->setWhatsThis(actionDescriptionHash[category]);
        }

        ++index;
    }

    return result;
}

const QPoint&
position()
{
    return position_;
}

const QSize&
resolution()
{
    return resolution_;
}

void
writePreferenceFile(const QString& filePath)
{
    QStringList categories;
    QStringList stringList;
    QList<QString>::const_iterator catIter;
    QList<QString>::const_iterator iter;
    QFile file(filePath);
    QXmlStreamWriter writer;
    QAction *action;
    int index;

    if (file.open(QIODevice::WriteOnly | QIODevice::Text |
            QIODevice::Truncate)) {
        writer.setDevice(&file);
        writer.setAutoFormatting(true);

        writer.writeStartDocument();

        // Root node.
        writer.writeStartElement("codedit_prefs");

        // Position and size.
        writer.writeStartElement("PositionSize");
        if (MainWindow::get().windowState() == Qt::WindowMaximized) {
            writer.writeAttribute("max", QString("1"));
        } else if (MainWindow::get().windowState() == Qt::WindowFullScreen) {
            writer.writeAttribute("full", QString("1"));
        } else {
            writer.writeAttribute("x",
                QString("%1").arg(MainWindow::get().pos().x()));
            writer.writeAttribute("y",
                QString("%1").arg(MainWindow::get().pos().y()));
            writer.writeAttribute("width",
                QString("%1").arg(MainWindow::get().size().width()));
            writer.writeAttribute("height",
                QString("%1").arg(MainWindow::get().size().height()));
        }
        writer.writeEndElement();

        // Font.
        writer.writeStartElement("Font");
        writer.writeAttribute("family", Editor::editorFont().family());
        writer.writeAttribute("size",
            QString("%1").arg(Editor::editorFont().pointSize()));
        writer.writeEndElement();

        // Tab size.
        writer.writeStartElement("TabSize");
        writer.writeAttribute("size", QString("%1").arg(Editor::tabSize()));
        writer.writeEndElement();

        // Word wrap.
        writer.writeStartElement("WordWrap");
        writer.writeAttribute("enabled", QString("%1").arg(Editor::wordWrap()));
        writer.writeEndElement();

        // Status bar.
        writer.writeStartElement("StatusBar");
        writer.writeAttribute("enabled",
            QString("%1").arg(MainWindow::get().isStatusBarVisible()));
        writer.writeEndElement();

        // Space for tab.
        writer.writeStartElement("SpaceForTab");
        writer.writeAttribute("enabled",
            QString("%1").arg(Editor::isSpaceForTab()));
        writer.writeEndElement();

        // Menu bar.
        writer.writeStartElement("MenuBar");
        writer.writeAttribute("enabled",
            QString("%1").arg(MainWindow::get().isMenuBarVisible()));
        writer.writeEndElement();

        // Line numbers.
        writer.writeStartElement("LineNumber");
        writer.writeAttribute("enabled",
            QString("%1").arg(Editor::isLineNumberEnabled()));
        writer.writeEndElement();

        // Line number border.
        writer.writeStartElement("LineNumberBorder");
        writer.writeAttribute("enabled",
            QString("%1").arg(Editor::isLineNumberBorderEnabled()));
        writer.writeEndElement();

        // Text/foreground color.
        writer.writeStartElement("TextColor");
        writer.writeAttribute("color", Editor::foregroundColor().name());
        writer.writeEndElement();

        // Keyword color.
        writer.writeStartElement("KeywordColor");
        writer.writeAttribute("color",
            SyntaxHighlighter::keywordColor().name());
        writer.writeEndElement();

        // Comment color.
        writer.writeStartElement("CommentColor");
        writer.writeAttribute("color",
            SyntaxHighlighter::commentColor().name());
        writer.writeEndElement();

        // Quotation color.
        writer.writeStartElement("QuotationColor");
        writer.writeAttribute("color",
            SyntaxHighlighter::quotationColor().name());
        writer.writeEndElement();

        // Background color.
        writer.writeStartElement("BackgroundColor");
        writer.writeAttribute("color", Editor::backgroundColor().name());
        writer.writeEndElement();

        // Highlight color.
        writer.writeStartElement("HighlightColor");
        writer.writeAttribute("color", Editor::highlightColor().name());
        writer.writeEndElement();

        // Current line color.
        writer.writeStartElement("CurrentLineColor");
        writer.writeAttribute("color", Editor::currentLineColor().name());
        writer.writeEndElement();

        // Nondocument color.
        writer.writeStartElement("NondocumentColor");
        writer.writeAttribute("color", Editor::nondocumentColor().name());
        writer.writeEndElement();

        // Character 80 indicator color.
        writer.writeStartElement("Character80Color");
        writer.writeAttribute("color", Editor::char80IndicatorColor().name());
        writer.writeEndElement();

        // Active line number text color.
        writer.writeStartElement("ActiveLineNumberTextColor");
        writer.writeAttribute("color",
            Editor::activeLineNumberForegroundColor().name());
        writer.writeEndElement();

        // Inactive line number text color.
        writer.writeStartElement("InactiveLineNumberTextColor");
        writer.writeAttribute("color",
            Editor::inactiveLineNumberForegroundColor().name());
        writer.writeEndElement();

        // Active line number background color.
        writer.writeStartElement("ActiveLineNumberBackgroundColor");
        writer.writeAttribute("color",
            Editor::activeLineNumberBackgroundColor().name());
        writer.writeEndElement();

        // Inactive line number background color.
        writer.writeStartElement("InactiveLineNumberBackgroundColor");
        writer.writeAttribute("color",
            Editor::inactiveLineNumberBackgroundColor().name());
        writer.writeEndElement();

        // Line number border color.
        writer.writeStartElement("LineNumberBorderColor");
        writer.writeAttribute("color", Editor::lineNumberBorderColor().name());
        writer.writeEndElement();

        // Keywords, comments, and extensions.
        categories = SyntaxHighlighter::categories();
        catIter = categories.constBegin();
        while (catIter != categories.constEnd()) {
            writer.writeStartElement("Category");
            writer.writeAttribute("cat", *catIter);

            // Single line comment.
            writer.writeStartElement("SingleLineComment");
            writer.writeAttribute("text",
                SyntaxHighlighter::singleLineComments(*catIter));
            writer.writeEndElement();

            // Multiline start comment.
            writer.writeStartElement("MultiLineStartComment");
            writer.writeAttribute("text",
                SyntaxHighlighter::multiLineStartComments(*catIter));
            writer.writeEndElement();

            // Multiline end comment.
            writer.writeStartElement("MultiLineEndComment");
            writer.writeAttribute("text",
                SyntaxHighlighter::multiLineEndComments(*catIter));
            writer.writeEndElement();

            // Keywords.
            writer.writeStartElement("Keywords");
            stringList = SyntaxHighlighter::keywords(*catIter);
            iter = stringList.constBegin();
            while (iter != stringList.constEnd()) {
                writer.writeTextElement("Keyword", *iter);
                ++iter;
            }
            writer.writeEndElement();

            // Extensions.
            writer.writeStartElement("Extensions");
            stringList = Document::extensions(*catIter);
            iter = stringList.constBegin();
            while (iter != stringList.constEnd()) {
                writer.writeTextElement("Extension", *iter);
                ++iter;
            }
            writer.writeEndElement();

            // Category end.
            writer.writeEndElement();

            ++catIter;
        }

        // Actions.
        index = 0;
        while (index < ActionEditor::count()) {
            action = ActionEditor::action(index);
            writer.writeStartElement("Action");
            writer.writeAttribute("ID", action->data().toString());
            writer.writeTextElement("MenuName", action->text());
            writer.writeTextElement("Shortcut", action->shortcut().toString());
            writer.writeTextElement("Description", action->whatsThis());
            writer.writeEndElement();

            ++index;
        }

        // Remote Save.
        writer.writeStartElement("RemoteSave");
            writer.writeStartElement("Address");
            writer.writeAttribute("text",
                PreferenceDialog::get().serverAddress());
            writer.writeEndElement();

            writer.writeStartElement("Port");
            writer.writeAttribute("text",
                QString("%1").arg(PreferenceDialog::get().serverPort()));
            writer.writeEndElement();

            writer.writeStartElement("PrivateKey");
            writer.writeAttribute("text",
                PreferenceDialog::get().serverPrivateKey());
            writer.writeEndElement();
        writer.writeEndElement();

        // Root node end.
        writer.writeEndElement();

        writer.writeEndDocument();

        file.close();
    }
}
