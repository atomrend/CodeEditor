#include "QuickOpenDialog.h"
#include "FileSystemLineEdit.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

QuickOpenDialog* QuickOpenDialog::dialog_ = NULL;

QuickOpenDialog::QuickOpenDialog(QWidget* parent) :
    QDialog(parent, Qt::Dialog),
    edit_(new FileSystemLineEdit(this))
{
    QPushButton *button;
    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout;
    QLabel *lbl = new QLabel(tr("File Path"));

    setWindowTitle(tr("Quick Open Dialog"));
    setLayout(vLayout);

    hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    button = new QPushButton(tr("OK"));
    hLayout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    button->setFocusPolicy(Qt::ClickFocus);

    button = new QPushButton(tr("Cancel"));
    hLayout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(reject()));
    button->setFocusPolicy(Qt::ClickFocus);

    hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    hLayout->addWidget(lbl);
    hLayout->addWidget(edit_);
    edit_->setFocusPolicy(Qt::ClickFocus);

    if (parent) {
        setFixedWidth(parent->width());
        move(parent->pos().x(), parent->pos().y() + parent->height() / 2);
    }
}

QStringList
QuickOpenDialog::paths() const
{
    return edit_->paths();
}

QString
QuickOpenDialog::directory() const
{
    return edit_->directory();
}

void
QuickOpenDialog::setCurrentPath(const QString& path)
{
    edit_->setText(path);
}

QuickOpenDialog&
QuickOpenDialog::get(QWidget* parent)
{
    if (!dialog_) {
        dialog_ = new QuickOpenDialog(parent);
    }

    return *dialog_;
}

int
QuickOpenDialog::exec()
{
    edit_->setFocus();

    return QDialog::exec();
}


