#include "ActionEditor.h"
#include <QAction>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeySequence>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

ActionEditor* ActionEditor::singleton_ = NULL;

ActionEditor::ActionEditor(QWidget* parent) :
    QWidget(parent),
    actions_(QVector<QAction*>()),
    statusLabel_(new QLabel("Status: ")),
    table_(new QTableWidget),
    previousValue_(QString())
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    table_->setColumnCount(3);
    table_->horizontalHeader()->setStretchLastSection(true);
    connect(table_, SIGNAL(itemChanged(QTableWidgetItem*)),
        this, SLOT(validate(QTableWidgetItem*)));
    connect(table_, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
        this, SLOT(saveOldValue(QTableWidgetItem*)));
    layout->addWidget(table_);

    layout->addWidget(statusLabel_);
}

ActionEditor::~ActionEditor()
{
}

void
ActionEditor::setup()
{
    QFontMetrics metrics(table_->font());
    int nameWidth = metrics.width("Name");
    int shortcutWidth = metrics.width("Shortcut");
    int width;
    QTableWidgetItem *nameItem;
    QTableWidgetItem *shortcutItem;
    QTableWidgetItem *descriptionItem;
    int rowIndex = 0;
    QVector<QAction*>::const_iterator iter = actions_.constBegin();

    table_->blockSignals(true);

    table_->clear();
    table_->setHorizontalHeaderLabels(QStringList() << "Name" <<
        "Shortcut" << "Description");
    table_->setRowCount(actions_.size());

    while (iter != actions_.constEnd()) {
        nameItem = new QTableWidgetItem((*iter)->text());
        nameItem->setToolTip((*iter)->text());
        table_->setItem(rowIndex, 0, nameItem);
        width = metrics.width((*iter)->text());
        if (width > nameWidth) {
            nameWidth = width;
        }

        shortcutItem = new QTableWidgetItem((*iter)->shortcut().toString());
        shortcutItem->setToolTip((*iter)->shortcut().toString());
        table_->setItem(rowIndex, 1, shortcutItem);
        width = metrics.width((*iter)->shortcut().toString());
        if (width > shortcutWidth) {
            shortcutWidth = width;
        }

        descriptionItem = new QTableWidgetItem((*iter)->whatsThis());
        descriptionItem->setToolTip((*iter)->whatsThis());
        table_->setItem(rowIndex, 2, descriptionItem);

        ++rowIndex;
        ++iter;
    }

    table_->setColumnWidth(0, nameWidth + 10);
    table_->setColumnWidth(1, shortcutWidth + 10);

    table_->blockSignals(false);

    statusLabel_->setText("Status: ");
    statusLabel_->setStyleSheet("");
}

void
ActionEditor::addAction(QAction* action)
{
    get()->actions_.append(action);
}

bool
ActionEditor::removeAction(QAction* action)
{
    bool result = false;
    int index = get()->actions_.indexOf(action);

    if (index != -1) {
        get()->actions_.remove(index);
        result = true;
    }

    return result;
}

int
ActionEditor::count()
{
    return get()->actions_.count();
}

QAction*
ActionEditor::action(int index)
{
    QAction *result = NULL;

    if (index >= 0 && index < count()) {
        result = get()->actions_.at(index);
    }

    return result;
}

ActionEditor*
ActionEditor::get()
{
    if (singleton_ == NULL) {
        singleton_ = new ActionEditor;
    }

    return singleton_;
}

void
ActionEditor::validate(QTableWidgetItem* item)
{
    if (item->column() > 1) {
        // Skip changes to the description.
        table_->blockSignals(true);
        item->setText(previousValue_);
        table_->blockSignals(false);

        return;
    }

    QAction *actn = action(item->row());
    if (!actn) {
        return;
    }

    if (item->column() == 0) {
        actn->setText(item->text());
    } else if (item->text().isEmpty()) {
        actn->setShortcut(QKeySequence());
    } else {
        QKeySequence shortcut(item->text());
        if (shortcut.isEmpty()) {
            statusLabel_->setText("Status: Invalid shortcut! "
                "Please try a different one.");
            statusLabel_->setStyleSheet("background: red;");
            table_->blockSignals(true);
            item->setText(previousValue_);
            table_->blockSignals(false);

            return;
        }

        bool alreadyUsed = false;
        QVector<QAction*>::const_iterator iter = actions_.constBegin();
        while (iter != actions_.constEnd()) {
            if (shortcut == (*iter)->shortcut()) {
                alreadyUsed = true;
                break;
            }

            ++iter;
        }

        if (alreadyUsed) {
            statusLabel_->setText("Status: Shortcut already used! "
                "Please try a different one.");
            statusLabel_->setStyleSheet("background:red;");
            table_->blockSignals(true);
            item->setText(previousValue_);
            table_->blockSignals(false);

            return;
        }

        table_->blockSignals(true);
        actn->setShortcut(shortcut);
        item->setText(shortcut.toString());
        table_->blockSignals(false);
    }
}

void
ActionEditor::saveOldValue(QTableWidgetItem* item)
{
    previousValue_ = item->text();
    statusLabel_->setText("Status: ");
    statusLabel_->setStyleSheet("");
}


