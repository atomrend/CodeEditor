#include "FileInfoDialog.h"
#include "Document.h"
#include "MainWindow.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include "StatusBar.h"
#include "SyntaxHighlighter.h"

#define NAME_COLUMN 0
#define CATEGORY_COLUMN 1
#define FILE_PATH_COLUMN 2
#define BROWSE_COLUMN 3
#define REMOTE_FILE_PATH_COLUMN 4

#define INVALID_ADDRESS 0
#define INVALID_PORT 1
#define MISSING_REMOTE_PATH 2

FileInfoDialog* FileInfoDialog::singleton_ = NULL;

FileInfoDialog::FileInfoDialog(QWidget* parent) :
    QDialog(parent),
    table_(new QTableWidget)
{
    if (parent) {
        move(parent->pos());
        resize(parent->size());
    }

    setWindowTitle(tr("File Info Dialog"));

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    table_->setColumnCount(5);
    table_->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    layout->addWidget(table_);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);
    connect(buttons, SIGNAL(accepted()), this, SLOT(onApply()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

FileInfoDialog&
FileInfoDialog::get(QWidget* parent)
{
    if (!singleton_) {
        singleton_ = new FileInfoDialog(parent);
    }

    return *singleton_;
}

int
FileInfoDialog::exec()
{
    int index = 0;
    QComboBox *comboBox;
    QPushButton *button;
    QTableWidgetItem *item;
    Document *doc;
    QStringList categories(SyntaxHighlighter::categories());
    QColor color;

    if (!categories.contains("Default")) {
        categories.append("Default");
    }
    categories.sort();

    table_->clear();
    table_->setHorizontalHeaderLabels(
        QStringList() << tr("Name") << tr("Category") << tr("File Path") << 
            QString() << tr("Remote File Path"));
    table_->setRowCount(Document::count());

    while (index < Document::count()) {
        doc = Document::document(index);
        if (doc != 0) {
            item = new QTableWidgetItem(doc->name());
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            table_->setItem(index, NAME_COLUMN, item);

            comboBox = new QComboBox;
            comboBox->setEditable(false);
            comboBox->addItems(categories);
            comboBox->setCurrentIndex(
                comboBox->findText(doc->category()));
            table_->setCellWidget(index, CATEGORY_COLUMN, comboBox);

            item = new QTableWidgetItem(doc->path());
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            table_->setItem(index, FILE_PATH_COLUMN, item);

            button = new QPushButton(tr("Browse..."));
            connect(button, SIGNAL(clicked()), this, SLOT(onBrowse()));
            table_->setCellWidget(index, BROWSE_COLUMN, button);

            item = new QTableWidgetItem(doc->remotePath());
            if (doc->remoteSaveSuccessful() == false) {
                color.setRgb(255, 127, 127, 255);
            } else {
                color.setRgb(255, 255, 255, 255);
            }
            item->setBackground(color);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            table_->setItem(index, REMOTE_FILE_PATH_COLUMN, item);
        }

        ++index;
    }

    table_->resizeColumnsToContents();
    table_->horizontalHeader()->setResizeMode(FILE_PATH_COLUMN,
        QHeaderView::Stretch);
    table_->horizontalHeader()->setResizeMode(REMOTE_FILE_PATH_COLUMN,
        QHeaderView::Stretch);

    return QDialog::exec();
}

void
FileInfoDialog::onApply()
{
    QComboBox *comboBox;
    Document *doc;
    int index = 0;

    index = 0;
    while (index < table_->rowCount()) {
        doc = Document::document(index);
        doc->setName(table_->item(index, NAME_COLUMN)->text());
        doc->setPath(table_->item(index, FILE_PATH_COLUMN)->text());
        comboBox = (QComboBox*) table_->cellWidget(index, CATEGORY_COLUMN);
        if (comboBox != 0) {
            doc->setCategory(comboBox->currentText());
        }
        doc->setRemotePath(table_->item(index, REMOTE_FILE_PATH_COLUMN)->text());

        ++index;
    }

    Document::sortDocumentList();
    MainWindow::get().updateDocumentsMenu();
    StatusBar::updateAllStatusBars();

    QDialog::accept();
}

void
FileInfoDialog::onBrowse()
{
    int index = 0;
    QPushButton *button = dynamic_cast<QPushButton*>(sender());
    QString filePath;

    if (button) {
        while (index < table_->rowCount()) {
            if (table_->cellWidget(index, BROWSE_COLUMN) == button) {
                break;
            }
            ++index;
        }

        if (index < table_->rowCount()) {
            filePath = QFileDialog::getSaveFileName();
            if (!filePath.isEmpty()) {
                table_->item(index, FILE_PATH_COLUMN)->setText(filePath);
                table_->resizeColumnsToContents();
                table_->horizontalHeader()->setResizeMode(FILE_PATH_COLUMN,
                    QHeaderView::Stretch);
            }
        }
    }
}
