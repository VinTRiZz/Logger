#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QFileDialog>

#define log(what) showStatus(what)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_pLoglistModel {new QStandardItemModel(this)},
    m_logColorDelegate {new LogColorDelegate(this)},
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->logs_tableView->horizontalHeader()->setHidden(true);
    ui->logs_tableView->verticalHeader()->setHidden(true);
    ui->logs_tableView->setModel(m_pLoglistModel);

    ui->logs_tableView->setItemDelegate(m_logColorDelegate);

    m_loggerCore.setLogChannel([this](const QString& msg){ showStatus(msg); });

    setupSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processFile()
{
    const QString filename = ui->logFileName_lineEdit->text();
    qDebug() << "Processing file:" << filename;
    if (filename.isEmpty())
    {
        qDebug() << "Error: Empty filename";
        log("Empty file name");
        return;
    }

    if (!m_loggerCore.parseFile(filename))
    {
        log("File invalid or not exist");
        return;
    }

    qDebug() << "Parsing complete";

    fillSessionList();
}

void MainWindow::acceptViewChanges()
{
    auto filterBuffer =  m_logTypeFilter;
    m_logTypeFilter.clear();

    // Log type filters
    if (!ui->showDebug_checkBox->isChecked())    m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_DEBUG);
    if (!ui->showInfo_checkBox->isChecked())     m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_INFO);
    if (!ui->showWarning_checkBox->isChecked())  m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_WARNING);
    if (!ui->showCritical_checkBox->isChecked()) m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_CRITICAL);
    if (!ui->showFatal_checkBox->isChecked())    m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_FATAL);

    // STD filters
    if (!ui->showCout_checkBox->isChecked())     m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_STDOUT);
    if (!ui->showCerr_checkBox->isChecked())     m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_STDERR);

    // Unknown logging filter
    if (!ui->showUnknown_checkBox->isChecked())  m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_UNKNOWN);

    if (filterBuffer != m_logTypeFilter)
        fillMessageList();
}

void MainWindow::updateLogTableContents()
{
    const QString choosenDate = ui->sessions_comboBox->currentText();

    m_loggerCore.resetDate();
    QString dateText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());

    if (dateText == choosenDate)
    {
        fillMessageList();
        return;
    }

    while (m_loggerCore.setNextDate())
    {
        dateText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());

        if (dateText == choosenDate)
        {
            fillMessageList();;
            return;
        }
    }
}

void MainWindow::chooseFileFromFilesystem()
{
    auto fileUrl = QFileDialog::getOpenFileUrl(this, "Aboba", QUrl("."), "*.txt Text file;; *.log Log journal file;; * Any other file");
    auto filePath = fileUrl.toLocalFile();

    if (!filePath.size())
        return;

    ui->logFileName_lineEdit->setText(filePath);
}

void MainWindow::setupSignals()
{
    connect(ui->chooseLogFile_pushButton, &QPushButton::clicked, this, &MainWindow::processFile);
    connect(ui->logFileName_lineEdit, &QLineEdit::returnPressed, this, &MainWindow::processFile);

    connect(ui->acceptShowSettings_pushButton, &QPushButton::clicked, this, &MainWindow::acceptViewChanges);
    connect(ui->sessions_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateLogTableContents);

    connect(ui->searchLogFile_pushButton, &QPushButton::clicked, this, &MainWindow::chooseFileFromFilesystem);
}

void MainWindow::showStatus(const QString &statusText)
{
    ui->status_label->setText(statusText);
}

void MainWindow::fillSessionList()
{
    // Clean from previous data
    m_pLoglistModel->clear();
    ui->sessions_comboBox->clear();

    if (m_loggerCore.logDateCount() < 1)
    {
        log("No sessions found in a file");
        qDebug() << "No sessions found";
        return;
    }

    // Add sessions loaded
    m_loggerCore.resetDate();
    const QString itemText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());
    ui->sessions_comboBox->addItem(itemText);

    while (m_loggerCore.setNextDate())
    {
        const QString itemText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());
        ui->sessions_comboBox->addItem(itemText);
    }
}

#define createColumnItem(itemText) \
    tempItem = new QStandardItem(itemText);\
    tempItem->setTextAlignment(Qt::AlignCenter); \
    columns.push_back(tempItem)

void MainWindow::fillMessageList()
{
    m_pLoglistModel->clear();

    // Setup header
    QList<QStandardItem*> columns;
    QStandardItem* tempItem;

    createColumnItem("Timestamp");
    createColumnItem("Log type");
    createColumnItem("File");
    createColumnItem("Function");
    createColumnItem("Text");

    m_pLoglistModel->appendRow(columns);

    m_loggerCore.resetMessageIndex();
    auto currentMessage = m_loggerCore.message();
    if (!currentMessage.use_count())
        return;

    while (m_loggerCore.setNextMessage())
    {
        if (m_logTypeFilter.contains(currentMessage->type)) // Skip anything in filter
        {
            currentMessage = m_loggerCore.message();
            continue;
        }

        columns.clear();

        createColumnItem(currentMessage->timestamp);
        createColumnItem(Logging::LogMessageStruct::typeString(currentMessage->type));
        createColumnItem(currentMessage->filestamp);
        createColumnItem(currentMessage->functionstamp);
        createColumnItem(currentMessage->text);

        m_pLoglistModel->appendRow(columns);

        currentMessage = m_loggerCore.message();
    }

    if (!m_logTypeFilter.contains(currentMessage->type)) // Skip anything in filter
    {
        columns.clear();

        createColumnItem(currentMessage->timestamp);
        createColumnItem(Logging::LogMessageStruct::typeString(currentMessage->type));
        createColumnItem(currentMessage->filestamp);
        createColumnItem(currentMessage->functionstamp);
        createColumnItem(currentMessage->text);

        m_pLoglistModel->appendRow(columns);
    }

    ui->logs_tableView->resizeColumnsToContents();
    auto header = ui->logs_tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    ui->logs_tableView->update();
}
