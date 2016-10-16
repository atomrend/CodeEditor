#include "StatusBar.h"

#include "Document.h"
#include "Editor.h"
#include <QComboBox>
#include <QHBoxLayout>  
#include <QLabel>

QVector<StatusBar*> StatusBar::statusBars_ = QVector<StatusBar*>();

StatusBar::StatusBar(Editor* editor, QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    lineNum_(1),
    colNum_(1),
    editor_(editor),
    positionLabel_(new QLabel),
    documentSelector_(new QComboBox)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    positionLabel_->setText(" Line: 1 Col: 1");
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    documentSelector_->setMaxVisibleItems(100);
    layout->addWidget(positionLabel_);
    layout->addWidget(documentSelector_, 1);
    connect(documentSelector_, SIGNAL(currentIndexChanged(int)), this,
        SIGNAL(documentSelected(int)));
    statusBars_.append(this);
}

StatusBar::~StatusBar()
{
    if (statusBars_.contains(this)) {
        statusBars_.remove(statusBars_.indexOf(this));
    }
}

void
StatusBar::setLineNumber(unsigned int val)
{
    lineNum_ = val;
    updatePosition();
}

void
StatusBar::setColumnNumber(unsigned int val)
{
    colNum_ = val;
    updatePosition();
}

void
StatusBar::updateDocumentList()
{
    int countDigits = 1;
    int val = Document::count() - 1;
    while (val >= 10) {
        val /= 10;
        ++countDigits;
    }

    documentSelector_->blockSignals(true);
    documentSelector_->clear();
    int currentIndex = -1;
    int currentDigits;
    QString text;
    Document *doc;
    for (int index = 0; index < Document::count(); ++index) {
        currentDigits = 1;
        val = index;
        while (val >= 10) {
            val /= 10;
            ++currentDigits;
        }

        text.clear();
        while (currentDigits < countDigits) {
            text.append('0');
            ++currentDigits;
        }
        text.append(QString("%1: ").arg(index));

        doc = Document::document(index);
        text.append(doc->name());
        documentSelector_->addItem(text);
        if (editor_->document() == doc) {
            currentIndex = index;
        }
    }
    if (currentIndex >= 0) {
        documentSelector_->setCurrentIndex(currentIndex);
    }
    documentSelector_->blockSignals(false);
}

int
StatusBar::count()
{
    return statusBars_.count();
}

StatusBar*
StatusBar::statusBar(int index)
{
    StatusBar *result = NULL;

    if (index >= 0 && index < count()) {
        result = statusBars_.at(index);
    }

    return result;
}

void
StatusBar::updateAllStatusBars()
{
    for (int index = 0; index < count(); ++index) {
        statusBars_.at(index)->updateDocumentList();
    }
}

void
StatusBar::changeSelectedDocument(int index)
{
    documentSelector_->blockSignals(true);
    documentSelector_->setCurrentIndex(index);
    documentSelector_->blockSignals(false);
}

void
StatusBar::updatePosition()
{
    positionLabel_->setText(
        QString(" Line: %1 Col: %2 ").arg(lineNum_).arg(colNum_));
}


