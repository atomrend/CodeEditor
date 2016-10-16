#include "SaveListDialog.h"
#include "Document.h"
#include <QAction>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QKeySequence>
#include <QPushButton>
#include <QTableWidget>
#include <QVariant>
#include <QVBoxLayout>

SaveListDialog* SaveListDialog::singleton_ = NULL;

SaveListDialog::SaveListDialog(QWidget* parent) :
    QDialog(parent),
    table_(new QTableWidget)
{
    QAction *action = new QAction(this);
    QVBoxLayout *layout = new QVBoxLayout;
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Discard |
        QDialogButtonBox::Cancel);

    setWindowTitle(tr("Save List"));
    setLayout(layout);

    table_->setColumnCount(1);
    table_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    layout->addWidget(table_);

    layout->addWidget(buttons);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons->button(QDialogButtonBox::Discard), SIGNAL(clicked()),
        this, SLOT(accept()));
    connect(buttons->button(QDialogButtonBox::Save), SIGNAL(clicked()),
        this, SLOT(onSave()));

    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    connect(action, SIGNAL(triggered()), this, SLOT(accept()));
    addAction(action);
}

SaveListDialog::~SaveListDialog()
{
}

SaveListDialog&
SaveListDialog::get(QWidget* parent)
{
    if (!singleton_) {
        singleton_ = new SaveListDialog(parent);
    }

    return *singleton_;
}

int
SaveListDialog::exec()
{
    int index = 0;
    QTableWidgetItem *item;
    Document *doc;

    table_->clear();
    table_->setRowCount(0);
    table_->setHorizontalHeaderLabels(QStringList() << tr("Name"));

    while (index < Document::count()) {
        doc = Document::document(index);
        if (doc != 0 && doc->needToSave()) {
            table_->setRowCount(table_->rowCount() + 1);

            item = new QTableWidgetItem(doc->name());
            item->setCheckState(Qt::Checked);
            item->setData(Qt::UserRole, qVariantFromValue((void*) doc));
            table_->setItem(table_->rowCount() - 1, 0, item);
        }

        ++index;
    }

    return QDialog::exec();
}

void
SaveListDialog::onSave()
{
    int index = 0;
    QTableWidgetItem *item = 0;
    Document *doc = 0;

    while (index < table_->rowCount()) {
        item = table_->item(index, 0);
        if (item->checkState() == Qt::Checked) {
            doc = (Document*) item->data(Qt::UserRole).value<void*>();
            if (doc != 0) {
                doc->save();
            }
        }

        ++index;
    }

    QDialog::accept();
}
