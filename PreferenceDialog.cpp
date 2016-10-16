#include "PreferenceDialog.h"
#include "ActionEditor.h"
#include "Document.h"
#include "Editor.h"
#include "MainWindow.h"
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFontDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QPushButton>
#include <QSpacerItem>
#include <QSvgRenderer>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include "SyntaxHighlighter.h"

#define REMOTE_PIXMAP_SIZE 24

PreferenceDialog* PreferenceDialog::preferenceDialog_ = NULL;

PreferenceDialog::PreferenceDialog(QWidget* parent) :
    QDialog(parent),
    font_(QFont()),
    fontButton_(new QPushButton),
    lineNumberBorder_(new QCheckBox("Line Number Border")),
    tabSize_(new QSpinBox),
    categories_(new QListWidget),
    newCategory_(new QLineEdit),
    categoryList_(QStringList()),
    textColorButton_(new QPushButton("Text Color")),
    keywordColorButton_(new QPushButton("Keyword Color")),
    commentColorButton_(new QPushButton("Comment Color")),
    quotationColorButton_(new QPushButton("Quotation Color")),
    backgroundColorButton_(new QPushButton("Background Color")),
    nondocumentColorButton_(new QPushButton("Nondocument Color")),
    char80ColorButton_(new QPushButton("80 Character Color")),
    highlightColorButton_(new QPushButton("Highlight Color")),
    currentLineColorButton_(new QPushButton("Current Line Color")),
    activeLineNumberTextColorButton_(
        new QPushButton("Active Line Number Text Color")),
    inactiveLineNumberTextColorButton_(
        new QPushButton("Inactive Line Number Text Color")),
    activeLineNumberBackgroundColorButton_(
        new QPushButton("Active Line Number Background Color")),
    inactiveLineNumberBackgroundColorButton_(
        new QPushButton("Inactive Line Number Background Color")),
    lineNumberBorderColorButton_(new QPushButton("Line Number Border Color")),
    keywordCategory_(new QComboBox),
    keywords_(new QListWidget),
    newKeyword_(new QLineEdit),
    keywordHash_(QHash<QString, QStringList>()),
    commentCategory_(new QComboBox),
    singleLineComment_(new QLineEdit),
    multiLineStartComment_(new QLineEdit),
    multiLineEndComment_(new QLineEdit),
    singleLineCommentHash_(QHash<QString, QString>()),
    multiLineStartCommentHash_(QHash<QString, QString>()),
    multiLineEndCommentHash_(QHash<QString, QString>()),
    extensionCategory_(new QComboBox),
    extensions_(new QListWidget),
    newExtension_(new QLineEdit),
    extensionHash_(QHash<QString, QStringList>()),
    serverAddress_(new QLineEdit),
    serverPort_(new QSpinBox),
    serverPrivateKey_(new QLineEdit),
    serverPassword_(new QLineEdit),
    serverAction_(new QLineEdit),
    remoteErrorStatus_(Success),
    remoteStatus_(new QLabel),
    remoteExplanation_(new QLabel),
    remoteSuccess_(QPixmap(REMOTE_PIXMAP_SIZE, REMOTE_PIXMAP_SIZE)),
    remoteFailure_(QPixmap(REMOTE_PIXMAP_SIZE, REMOTE_PIXMAP_SIZE))
{
    QWidget *widget;
    QGridLayout *tabLayout;
    QHBoxLayout *hLayout;
    QLabel *label;
    QPushButton *button;
    QVBoxLayout *layout = new QVBoxLayout;
    QTabWidget *tabWidget = new QTabWidget;
    QSvgRenderer svg;
    QPainter painter;

    int row;
    if (parent != 0) {
        move(parent->pos());
        resize(parent->size());
    }

    setWindowTitle(tr("Codedit Preferences", "Codedit Preferences"));

    setLayout(layout);

    tabWidget->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(tabWidget);

    ////////////////////////////////////////////////////////////////////////////
    // General options.
    widget = new QWidget;
    tabWidget->addTab(widget, tr("General", "General"));
    tabLayout = new QGridLayout;
    tabLayout->setColumnStretch(0, 1);
    tabLayout->setColumnStretch(3, 1);
    widget->setLayout(tabLayout);

    // Font.
    hLayout = new QHBoxLayout;
    tabLayout->addLayout(hLayout, 0, 1, 1, 2);
    label = new QLabel(tr("Font:", "Font:"));
    fontButton_->setFocusPolicy(Qt::NoFocus);
    hLayout->addWidget(label, 0);
    hLayout->addWidget(fontButton_, 1);
    connect(fontButton_, SIGNAL(clicked()), this, SLOT(onFont()));

    // Line number border toggle.
    lineNumberBorder_->setFocusPolicy(Qt::NoFocus);
    tabLayout->addWidget(lineNumberBorder_, 1, 1);

    // Tab size.
    hLayout = new QHBoxLayout;
    tabLayout->addLayout(hLayout, 2, 1, 1, 2);
    label = new QLabel(tr("Tab Size:", "Tab Size:"));
    hLayout->addWidget(label, 0);
    hLayout->addWidget(tabSize_, 1);

    tabLayout->addItem(
        new QSpacerItem(1, 24, QSizePolicy::Expanding, QSizePolicy::Fixed),
        3, 0);

    // Categories.
    label = new QLabel(tr("Categories:", "Categories:"));
    tabLayout->addWidget(label, 4, 1);

    tabLayout->addWidget(categories_, 5, 1, 1, 2);
    categories_->setMaximumHeight(200);

    hLayout = new QHBoxLayout;
    tabLayout->addLayout(hLayout, 6, 1, 1, 2);

    hLayout->addWidget(newCategory_, 1);
    connect(newCategory_, SIGNAL(returnPressed()),
        this, SLOT(onAddCategory()));

    button = new QPushButton("+");
    button->setFixedWidth(35);
    button->setToolTip(tr("Add Category", "Add Category"));
    hLayout->addWidget(button, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(onAddCategory()));

    button = new QPushButton("-");
    button->setFixedWidth(35);
    button->setToolTip(tr("Remove Category", "Remove Category"));
    hLayout->addWidget(button, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(onRemoveCategory()));

    tabLayout->addItem(
        new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding),
        7, 0);

    ////////////////////////////////////////////////////////////////////////////
    // Color options.
    widget = new QWidget;
    tabWidget->addTab(widget, tr("Colors", "Colors"));
    tabLayout = new QGridLayout;
    tabLayout->setColumnStretch(0, 1);
    tabLayout->setColumnStretch(2, 1);
    widget->setLayout(tabLayout);

    tabLayout->addWidget(textColorButton_, 0, 1);
    textColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(textColorButton_, SIGNAL(clicked()),
        this, SLOT(onTextColor()));

    tabLayout->addWidget(keywordColorButton_, 1, 1);
    keywordColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(keywordColorButton_, SIGNAL(clicked()),
        this, SLOT(onKeywordColor()));

    tabLayout->addWidget(commentColorButton_, 2, 1);
    commentColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(commentColorButton_, SIGNAL(clicked()),
        this, SLOT(onCommentColor()));

    tabLayout->addWidget(quotationColorButton_, 3, 1);
    quotationColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(quotationColorButton_, SIGNAL(clicked()),
        this, SLOT(onQuotationColor()));

    tabLayout->addWidget(backgroundColorButton_, 4, 1);
    backgroundColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(backgroundColorButton_, SIGNAL(clicked()),
        this, SLOT(onBgColor()));

    tabLayout->addWidget(nondocumentColorButton_, 5, 1);
    nondocumentColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(nondocumentColorButton_, SIGNAL(clicked()),
        this, SLOT(onNondocColor()));

    tabLayout->addWidget(char80ColorButton_, 6, 1);
    char80ColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(char80ColorButton_, SIGNAL(clicked()),
        this, SLOT(onChar80Color()));

    tabLayout->addWidget(highlightColorButton_, 7, 1);
    highlightColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(highlightColorButton_, SIGNAL(clicked()),
        this, SLOT(onHiliteColor()));

    tabLayout->addWidget(currentLineColorButton_, 8, 1);
    currentLineColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(currentLineColorButton_, SIGNAL(clicked()),
        this, SLOT(onCurrentLineColor()));

    tabLayout->addWidget(activeLineNumberTextColorButton_, 9, 1);
    activeLineNumberTextColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(activeLineNumberTextColorButton_, SIGNAL(clicked()),
        this, SLOT(onActiveLineNumTextColor()));

    tabLayout->addWidget(inactiveLineNumberTextColorButton_, 10, 1);
    inactiveLineNumberTextColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(inactiveLineNumberTextColorButton_, SIGNAL(clicked()),
        this, SLOT(onInactiveLineNumTextColor()));

    tabLayout->addWidget(activeLineNumberBackgroundColorButton_, 11, 1);
    activeLineNumberBackgroundColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(activeLineNumberBackgroundColorButton_, SIGNAL(clicked()),
        this, SLOT(onActiveLineNumBgColor()));

    tabLayout->addWidget(inactiveLineNumberBackgroundColorButton_, 12, 1);
    inactiveLineNumberBackgroundColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(inactiveLineNumberBackgroundColorButton_, SIGNAL(clicked()),
        this, SLOT(onInactiveLineNumBgColor()));

    tabLayout->addWidget(lineNumberBorderColorButton_, 13, 1);
    lineNumberBorderColorButton_->setFocusPolicy(Qt::NoFocus);
    connect(lineNumberBorderColorButton_, SIGNAL(clicked()),
        this, SLOT(onLineNumBorderColor()));

    tabLayout->addItem(
        new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding),
        14, 0);

    ////////////////////////////////////////////////////////////////////////////
    // Keyword options.
    widget = new QWidget;
    tabWidget->addTab(widget, tr("Keywords", "Keywords"));
    tabLayout = new QGridLayout;
    tabLayout->setColumnStretch(0, 1);
    tabLayout->setColumnStretch(2, 1);
    widget->setLayout(tabLayout);

    hLayout = new QHBoxLayout;
    label = new QLabel(tr("Category:", "Category:"));
    hLayout->addWidget(label, 0);
    hLayout->addWidget(keywordCategory_, 1);
    tabLayout->addLayout(hLayout, 0, 1);
    connect(keywordCategory_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onKeywordCategory(const QString&)));

    tabLayout->addWidget(keywords_, 1, 1);
    tabLayout->setRowStretch(1, 1);

    hLayout = new QHBoxLayout;
    tabLayout->addLayout(hLayout, 2, 1);
    hLayout->addWidget(newKeyword_, 1);

    button = new QPushButton("+");
    button->setFixedWidth(35);
    button->setToolTip(tr("Add Keyword", "Add Keyword"));
    hLayout->addWidget(button, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(onAddKeyword()));

    button = new QPushButton("-");
    button->setFixedWidth(35);
    button->setToolTip(tr("Remove Keyword", "Remove Keyword"));
    hLayout->addWidget(button, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(onRemoveKeyword()));

    ////////////////////////////////////////////////////////////////////////////
    // Comment options.
    widget = new QWidget;
    tabWidget->addTab(widget, tr("Comments", "Comments"));
    tabLayout = new QGridLayout;
    tabLayout->setColumnStretch(0, 1);
    tabLayout->setColumnStretch(3, 1);
    widget->setLayout(tabLayout);

    hLayout = new QHBoxLayout;
    label = new QLabel(tr("Category:", "Category:"));
    hLayout->addWidget(label, 0);
    hLayout->addWidget(commentCategory_, 1);
    tabLayout->addLayout(hLayout, 0, 1, 1, 2);
    connect(commentCategory_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onCommentCategory(const QString&)));

    // Single line.
    label = new QLabel(tr("Single Line:", "Single Line:"));
    tabLayout->addWidget(label, 1, 1);
    tabLayout->addWidget(singleLineComment_, 1, 2);
    connect(singleLineComment_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onSingleLineComment(const QString&)));

    // Multiline Start.
    label = new QLabel(tr("Multiline Start:", "Multiline Start:"));
    tabLayout->addWidget(label, 2, 1);
    tabLayout->addWidget(multiLineStartComment_, 2, 2);
    connect(multiLineStartComment_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onMultilineStartComment(const QString&)));

    // Multiline End.
    label = new QLabel(tr("Multiline End:", "Multiline End:"));
    tabLayout->addWidget(label, 3, 1);
    tabLayout->addWidget(multiLineEndComment_, 3, 2);
    connect(multiLineEndComment_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onMultilineEndComment(const QString&)));

    tabLayout->addItem(
        new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding),
        4, 0);

    ////////////////////////////////////////////////////////////////////////////
    // File extension options.
    widget = new QWidget;
    tabWidget->addTab(widget, tr("Extensions", "Extensions"));
    tabLayout = new QGridLayout;
    tabLayout->setColumnStretch(0, 1);
    tabLayout->setColumnStretch(2, 1);
    widget->setLayout(tabLayout);

    hLayout = new QHBoxLayout;
    label = new QLabel(tr("Category:", "Category:"));
    hLayout->addWidget(label, 0);
    hLayout->addWidget(extensionCategory_, 1);
    tabLayout->addLayout(hLayout, 0, 1);
    connect(extensionCategory_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onExtensionCategory(const QString&)));

    tabLayout->addWidget(extensions_, 1, 1);

    hLayout = new QHBoxLayout;
    tabLayout->addLayout(hLayout, 2, 1);
    hLayout->addWidget(newExtension_, 1);

    button = new QPushButton("+");
    button->setFixedWidth(35);
    button->setToolTip(tr("Add Extension", "Add Extension"));
    hLayout->addWidget(button, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(onAddExtension()));

    button = new QPushButton("-");
    button->setFixedWidth(35);
    button->setToolTip(tr("Remove Extension", "Remove Extension"));
    hLayout->addWidget(button, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(onRemoveExtension()));

    tabLayout->addItem(
        new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding),
        3, 0);

    ////////////////////////////////////////////////////////////////////////////
    // Keystroke options.
    tabWidget->addTab(ActionEditor::get(), tr("Keystrokes", "Keystrokes"));

    ////////////////////////////////////////////////////////////////////////////
    // Server options.
    widget = new QWidget;
    tabWidget->addTab(widget, tr("Server", "Server"));
    tabLayout = new QGridLayout;
    tabLayout->setColumnStretch(0, 0);
    tabLayout->setColumnStretch(1, 0);
    tabLayout->setColumnStretch(2, 1);
    widget->setLayout(tabLayout);
    row = 0;

    // Server Address.
    label = new QLabel(tr("Server Address", "Server Address"));
    tabLayout->addWidget(label, row, 0, 1, 1);
    tabLayout->addWidget(serverAddress_, row, 1, 1, 2);
    ++row;

    // Server Port.
    label = new QLabel(tr("Server Port", "Server Port"));
    tabLayout->addWidget(label, row, 0, 1, 1);
    serverPort_->setRange(1024, 65535);
    tabLayout->addWidget(serverPort_, row, 1, 1, 2);
    ++row;

    // Server Private Key.
    label = new QLabel(tr("Server Private Key", "Server Private Key"));
    tabLayout->addWidget(label, row, 0, 1, 1);
    serverPrivateKey_->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    tabLayout->addWidget(serverPrivateKey_, row, 1, 1, 2);
    ++row;

    // Server Password.
    label = new QLabel(tr("Server Password", "Server Password"));
    tabLayout->addWidget(label, row, 0, 1, 1);
    serverPassword_->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    tabLayout->addWidget(serverPassword_, row, 1, 1, 2);
    ++row;

    // Server Action.
    label = new QLabel(tr("Server Action", "Server Action"));
    tabLayout->addWidget(label, row, 0, 1, 1);
    serverAction_->setMaxLength(1460);
    tabLayout->addWidget(serverAction_, row, 1, 1, 2);
    ++row;

    // Remote Status.
    label = new QLabel(tr("Remote Server Status:", "Remote Server Status:"));
    tabLayout->addWidget(label, row, 0, 1, 1, Qt::AlignRight);
    tabLayout->addWidget(remoteStatus_, row, 1, 1, 1, Qt::AlignLeft);
    tabLayout->addWidget(remoteExplanation_, row, 2, 1, 1, Qt::AlignLeft);
    remoteExplanation_->setText(tr("Successful."));
    ++row;

    // Remote Status indicator icons.
    if (svg.load(QString(":/remoteSuccess.svg")) == true) {
        remoteSuccess_.fill(QColor(Qt::transparent));
        painter.begin(&remoteSuccess_);
        svg.render(&painter);
        painter.end();
        remoteStatus_->setPixmap(remoteSuccess_);
    }
    if (svg.load(QString(":/remoteFail.svg")) == true) {
        remoteFailure_.fill(QColor(Qt::transparent));
        painter.begin(&remoteFailure_);
        svg.render(&painter);
        painter.end();
    }

    tabLayout->setRowStretch(row, 1);

    ////////////////////////////////////////////////////////////////////////////
    // Ok and Cancel buttons.
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(onApply()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);
}

PreferenceDialog::~PreferenceDialog()
{
}

QString
PreferenceDialog::serverAddress() const
{
    return serverAddress_->text();
}

void
PreferenceDialog::setServerAddress(const QString& address)
{
    serverAddress_->setText(address);
}

int
PreferenceDialog::serverPort() const
{
    return serverPort_->value();
}

void
PreferenceDialog::setServerPort(const QString& port)
{
    bool ok;
    int value = port.toInt(&ok);

    if (ok == true) {
        serverPort_->setValue(value);
    }
}

QString
PreferenceDialog::serverPrivateKey() const
{
    return serverPrivateKey_->text();
}

void
PreferenceDialog::setServerPrivateKey(const QString& key)
{
    serverPrivateKey_->setText(key);
}

QString
PreferenceDialog::serverPassword() const
{
    return serverPassword_->text();
}

QString
PreferenceDialog::serverAction() const
{
    return serverAction_->text();
}

PreferenceDialog::RemoteErrorCodes
PreferenceDialog::remoteErrorStatus() const
{
    return remoteErrorStatus_;
}

void
PreferenceDialog::setRemoteErrorStatus(RemoteErrorCodes status)
{
    remoteErrorStatus_ = status;
}

PreferenceDialog&
PreferenceDialog::get(QWidget* parent)
{
    if (preferenceDialog_ == NULL) {
        preferenceDialog_ = new PreferenceDialog(parent);
    }

    return *preferenceDialog_;
}

int
PreferenceDialog::exec()
{
    int index;
    QString cat;
    QStringList stringList;

    move(MainWindow::get().pos());
    resize(MainWindow::get().size());

    font_ = Editor::editorFont();
    fontButton_->setText(
        QString("%1 @ %2").arg(font_.family()).arg(font_.pointSize()));

    lineNumberBorder_->setCheckState(Editor::isLineNumberBorderEnabled() ?
        Qt::Checked : Qt::Unchecked);

    tabSize_->setValue(Editor::tabSize());

    categoryList_ = SyntaxHighlighter::categories();
    categoryList_.sort();
    categories_->clear();
    categories_->addItems(categoryList_);

    index = 0;
    while (index < categoryList_.size()) {
        cat = categoryList_[index];
        stringList = SyntaxHighlighter::keywords(cat);
        stringList.sort();
        keywordHash_[cat] = stringList;
        singleLineCommentHash_[cat] = 
            SyntaxHighlighter::singleLineComments(cat);
        multiLineStartCommentHash_[cat] =
            SyntaxHighlighter::multiLineStartComments(cat);
        multiLineEndCommentHash_[cat] =
            SyntaxHighlighter::multiLineEndComments(cat);
        stringList = Document::extensions(cat);
        stringList.sort();
        extensionHash_[cat] = stringList;
        ++index;
    }

    keywordCategory_->clear();
    keywordCategory_->addItems(categoryList_);
    keywordCategory_->setCurrentIndex(keywordCategory_->findText("Default"));

    commentCategory_->clear();
    commentCategory_->addItems(categoryList_);
    commentCategory_->setCurrentIndex(commentCategory_->findText("Default"));

    extensionCategory_->clear();
    extensionCategory_->addItems(categoryList_);
    extensionCategory_->setCurrentIndex(
        extensionCategory_->findText("Default"));

    textColor_ = Editor::foregroundColor();
    setButtonColor(textColor_, textColorButton_);

    keywordColor_ = SyntaxHighlighter::keywordColor();
    setButtonColor(keywordColor_, keywordColorButton_);

    commentColor_ = SyntaxHighlighter::commentColor();
    setButtonColor(commentColor_, commentColorButton_);

    quotationColor_ = SyntaxHighlighter::quotationColor();
    setButtonColor(quotationColor_, quotationColorButton_);

    backgroundColor_ = Editor::backgroundColor();
    setButtonColor(backgroundColor_, backgroundColorButton_);

    nondocumentColor_ = Editor::nondocumentColor();
    setButtonColor(nondocumentColor_, nondocumentColorButton_);

    char80Color_ = Editor::char80IndicatorColor();
    setButtonColor(char80Color_, char80ColorButton_);

    highlightColor_ = Editor::highlightColor();
    setButtonColor(highlightColor_, highlightColorButton_);

    currentLineColor_ = Editor::currentLineColor();
    setButtonColor(currentLineColor_, currentLineColorButton_);

    activeLineNumberTextColor_ = Editor::activeLineNumberForegroundColor();
    setButtonColor(activeLineNumberTextColor_,
        activeLineNumberTextColorButton_);

    inactiveLineNumberTextColor_ = Editor::inactiveLineNumberForegroundColor();
    setButtonColor(inactiveLineNumberTextColor_,
        inactiveLineNumberTextColorButton_);

    activeLineNumberBackgroundColor_ =
        Editor::activeLineNumberBackgroundColor();
    setButtonColor(activeLineNumberBackgroundColor_,
        activeLineNumberBackgroundColorButton_);

    inactiveLineNumberBackgroundColor_ =
        Editor::inactiveLineNumberBackgroundColor();
    setButtonColor(inactiveLineNumberBackgroundColor_,
        inactiveLineNumberBackgroundColorButton_);

    lineNumberBorderColor_ = Editor::lineNumberBorderColor();
    setButtonColor(lineNumberBorderColor_, lineNumberBorderColorButton_);

    ActionEditor::get()->setup();

    switch (remoteErrorStatus_) {
    case Success:
        remoteStatus_->setPixmap(remoteSuccess_);
        remoteExplanation_->setText(tr("Successful."));
        break;
    case Connection:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(tr("Unable to connect to the server!"));
        break;
    case TimeOut:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(tr("Attempt to connect timed out!"));
        break;
    case Authentication:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(tr("Authentication failed!"));
        break;
    case FilePathTooLong:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(
            tr("File path for the remote file is too long!"));
        break;
    case CannotOpen:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(
            tr("Unable to open the remote file for saving!"));
        break;
    case BadFileSize:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(
            tr("Remote file size does not match the local file size!"));
        break;
    case BadFileChecksum:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(tr("Checksum failed!"));
        break;
    case FailedAction:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(tr("Remote action failed!"));
        break;
    case Port:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(tr("Invalid port number!"));
        break;
    case Address:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(tr("Invalid address!"));
        break;
    case InvalidReceiveSize:
        remoteStatus_->setPixmap(remoteFailure_);
        remoteExplanation_->setText(
            tr("Received an invalid size from the server!"));
        break;
    default:
        break;
    }

    return QDialog::exec();
}

void
PreferenceDialog::onApply()
{
    int index;
    QString cat;

    Editor::setEditorFont(font_);

    Editor::setLineNumberBorderEnabled(
        lineNumberBorder_->checkState() == Qt::Checked);

    Editor::setTabSize(tabSize_->value());

    Editor::setForegroundColor(textColor_);
    SyntaxHighlighter::setKeywordColor(keywordColor_);
    SyntaxHighlighter::setCommentColor(commentColor_);
    SyntaxHighlighter::setQuotationColor(quotationColor_);
    Editor::setBackgroundColor(backgroundColor_);
    Editor::setNondocumentColor(nondocumentColor_);
    Editor::setChar80IndicatorColor(char80Color_);
    Editor::setHighlightColor(highlightColor_);
    Editor::setCurrentLineColor(currentLineColor_);
    Editor::setActiveLineNumberForegroundColor(activeLineNumberTextColor_);
    Editor::setInactiveLineNumberForegroundColor(inactiveLineNumberTextColor_);
    Editor::setActiveLineNumberBackgroundColor(
        activeLineNumberBackgroundColor_);
    Editor::setInactiveLineNumberBackgroundColor(
        inactiveLineNumberBackgroundColor_);
    Editor::setLineNumberBorderColor(lineNumberBorderColor_);

    SyntaxHighlighter::clearKeywords();
    SyntaxHighlighter::clearComments();
    Document::clearExtensions();

    index = 0;
    while (index < categoryList_.size()) {
        cat = categoryList_[index];
        SyntaxHighlighter::setKeywords(cat, keywordHash_[cat]);
        SyntaxHighlighter::setSingleLineComments(cat,
            singleLineCommentHash_[cat]);
        SyntaxHighlighter::setMultiLineComments(cat,
            multiLineStartCommentHash_[cat], multiLineEndCommentHash_[cat]);
        Document::setExtensions(cat, extensionHash_[cat]);
        ++index;
    }

    Document::rehighlightSyntaxAllDocuments();

    QDialog::accept();
}

void
PreferenceDialog::onFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, font_, this);
    if (ok) {
        font_ = font;
        fontButton_->setText(
            QString("%1 @ %2").arg(font_.family()).arg(font_.pointSize()));
    }
}

void
PreferenceDialog::onAddCategory()
{
    if (!newCategory_->text().isEmpty()) {
        categoryList_.append(newCategory_->text());
        categoryList_.sort();
        populateCategories();
        newCategory_->clear();
    }
}

void
PreferenceDialog::onRemoveCategory()
{
    QList<QListWidgetItem*> items = categories_->selectedItems();
    QList<QListWidgetItem*>::const_iterator iter = items.constBegin();

    while (iter != items.constEnd()) {
        if ((*iter)->text() == "Default") {
            // The default category cannot be removed.
            categoryList_.removeAt(categoryList_.indexOf((*iter)->text()));
        }
        ++iter;
    }

    populateCategories();
}

void
PreferenceDialog::onTextColor()
{
    QColor color = QColorDialog::getColor(textColor_);
    if (color.isValid()) {
        textColor_ = color;
        setButtonColor(color, textColorButton_);
    }
}

void
PreferenceDialog::onKeywordColor()
{
    QColor color = QColorDialog::getColor(keywordColor_);
    if (color.isValid()) {
        keywordColor_ = color;
        setButtonColor(color, keywordColorButton_);
    }
}

void
PreferenceDialog::onCommentColor()
{
    QColor color = QColorDialog::getColor(commentColor_);
    if (color.isValid()) {
        commentColor_ = color;
        setButtonColor(color, commentColorButton_);
    }
}

void
PreferenceDialog::onQuotationColor()
{
    QColor color = QColorDialog::getColor(quotationColor_);
    if (color.isValid()) {
        quotationColor_ = color;
        setButtonColor(color, quotationColorButton_);
    }
}

void
PreferenceDialog::onBgColor()
{
    QColor color = QColorDialog::getColor(backgroundColor_);
    if (color.isValid()) {
        backgroundColor_ = color;
        setButtonColor(color, backgroundColorButton_);
    }
}

void
PreferenceDialog::onNondocColor()
{
    QColor color = QColorDialog::getColor(nondocumentColor_);
    if (color.isValid()) {
        nondocumentColor_ = color;
        setButtonColor(color, nondocumentColorButton_);
    }
}

void
PreferenceDialog::onChar80Color()
{
    QColor color = QColorDialog::getColor(char80Color_);
    if (color.isValid()) {
        char80Color_ = color;
        setButtonColor(color, char80ColorButton_);
    }
}

void
PreferenceDialog::onHiliteColor()
{
    QColor color = QColorDialog::getColor(highlightColor_);
    if (color.isValid()) {
        highlightColor_ = color;
        setButtonColor(color, highlightColorButton_);
    }
}

void
PreferenceDialog::onCurrentLineColor()
{
    QColor color = QColorDialog::getColor(currentLineColor_);
    if (color.isValid()) {
        currentLineColor_ = color;
        setButtonColor(color, currentLineColorButton_);
    }
}

void
PreferenceDialog::onActiveLineNumTextColor()
{
    QColor color = QColorDialog::getColor(activeLineNumberTextColor_);
    if (color.isValid()) {
        activeLineNumberTextColor_ = color;
        setButtonColor(color, activeLineNumberTextColorButton_);
    }
}

void
PreferenceDialog::onInactiveLineNumTextColor()
{
    QColor color = QColorDialog::getColor(inactiveLineNumberTextColor_);
    if (color.isValid()) {
        inactiveLineNumberTextColor_ = color;
        setButtonColor(color, inactiveLineNumberTextColorButton_);
    }
}

void
PreferenceDialog::onActiveLineNumBgColor()
{
    QColor color = QColorDialog::getColor(activeLineNumberBackgroundColor_);
    if (color.isValid()) {
        activeLineNumberBackgroundColor_ = color;
        setButtonColor(color, activeLineNumberBackgroundColorButton_);
    }
}

void
PreferenceDialog::onInactiveLineNumBgColor()
{
    QColor color = QColorDialog::getColor(inactiveLineNumberBackgroundColor_);
    if (color.isValid()) {
        inactiveLineNumberBackgroundColor_ = color;
        setButtonColor(color, inactiveLineNumberBackgroundColorButton_);
    }
}

void
PreferenceDialog::onLineNumBorderColor()
{
    QColor color = QColorDialog::getColor(lineNumberBorderColor_);
    if (color.isValid()) {
        lineNumberBorderColor_ = color;
        setButtonColor(color, lineNumberBorderColorButton_);
    }
}

void
PreferenceDialog::onKeywordCategory(const QString& text)
{
    keywords_->clear();
    if (keywordHash_.contains(text)) {
        keywords_->addItems(keywordHash_[text]);
    }
}

void
PreferenceDialog::onAddKeyword()
{
    if (!newKeyword_->text().isEmpty()) {
        QStringList stringList = keywordHash_[keywordCategory_->currentText()];
        stringList.append(newKeyword_->text());
        stringList.sort();
        keywordHash_[keywordCategory_->currentText()] = stringList;
        keywords_->clear();
        keywords_->addItems(stringList);
        newKeyword_->clear();
    }
}

void
PreferenceDialog::onRemoveKeyword()
{
    QStringList stringList = keywordHash_[keywordCategory_->currentText()];
    QList<QListWidgetItem*> items = keywords_->selectedItems();
    QList<QListWidgetItem*>::const_iterator iter = items.constBegin();

    while (iter != items.constEnd()) {
        stringList.removeAt(stringList.indexOf((*iter)->text()));
        ++iter;
    }

    stringList.sort();
    keywordHash_[keywordCategory_->currentText()] = stringList;
    keywords_->clear();
    keywords_->addItems(stringList);
}

void
PreferenceDialog::onCommentCategory(const QString& text)
{
    if (singleLineCommentHash_.contains(text)) {
        singleLineComment_->setText(singleLineCommentHash_[text]);
    }
    if (multiLineStartCommentHash_.contains(text)) {
        multiLineStartComment_->setText(multiLineStartCommentHash_[text]);
    }
    if (multiLineEndCommentHash_.contains(text)) {
        multiLineEndComment_->setText(multiLineEndCommentHash_[text]);
    }
}

void
PreferenceDialog::onSingleLineComment(const QString& text)
{
    singleLineCommentHash_[commentCategory_->currentText()] = text;
}

void
PreferenceDialog::onMultilineStartComment(const QString& text)
{
    multiLineStartCommentHash_[commentCategory_->currentText()] = text;
}

void
PreferenceDialog::onMultilineEndComment(const QString& text)
{
    multiLineEndCommentHash_[commentCategory_->currentText()] = text;
}

void
PreferenceDialog::onExtensionCategory(const QString& text)
{
    extensions_->clear();
    if (extensionHash_.contains(text)) {
        extensions_->addItems(extensionHash_[text]);
    }
}

void
PreferenceDialog::onAddExtension()
{
    if (!newExtension_->text().isEmpty()) {
        QStringList stringList =
            extensionHash_[extensionCategory_->currentText()];
        stringList.append(newExtension_->text());
        stringList.sort();
        extensionHash_[extensionCategory_->currentText()] = stringList;
        extensions_->clear();
        extensions_->addItems(stringList);
        newExtension_->clear();
    }
}

void
PreferenceDialog::onRemoveExtension()
{
    QStringList stringList = extensionHash_[extensionCategory_->currentText()];
    QList<QListWidgetItem*> items = extensions_->selectedItems();
    QList<QListWidgetItem*>::const_iterator iter = items.constBegin();

    while (iter != items.constEnd()) {
        stringList.removeAt(stringList.indexOf((*iter)->text()));
        ++iter;
    }

    stringList.sort();
    extensionHash_[extensionCategory_->currentText()] = stringList;
    extensions_->clear();
    extensions_->addItems(stringList);
}

void
PreferenceDialog::setButtonColor(const QColor& color, QPushButton* button)
{
    QString style =
        QString("QPushButton {background-color: %1;}").arg(color.name());
    button->setStyleSheet(style);

    QPalette palette(button->palette());
    if (color.lightness() > 128) {
        palette.setColor(QPalette::ButtonText, Qt::black);
    } else {
        palette.setColor(QPalette::ButtonText, Qt::white);
    }
    button->setPalette(palette);
}

void
PreferenceDialog::populateCategories()
{
    categories_->clear();
    categories_->addItems(categoryList_);

    keywordCategory_->clear();
    keywordCategory_->addItems(categoryList_);
    keywordCategory_->setCurrentIndex(keywordCategory_->findText("Default"));

    commentCategory_->clear();
    commentCategory_->addItems(categoryList_);
    commentCategory_->setCurrentIndex(commentCategory_->findText("Default"));

    extensionCategory_->clear();
    extensionCategory_->addItems(categoryList_);
    extensionCategory_->setCurrentIndex(
        extensionCategory_->findText("Default"));
}


