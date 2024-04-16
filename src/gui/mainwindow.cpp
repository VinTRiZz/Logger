#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#define log(what) showStatus(what)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_pLoglistModel {new QStandardItemModel(this)},
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->logs_tableView->setModel(m_pLoglistModel);

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
    log("Parsing complete");

    fillSessionList();
}

void MainWindow::acceptViewChanges()
{
    m_logTypeFilter.clear();

    if (!ui->showDebug_checkBox->isChecked())    m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_DEBUG);
    if (!ui->showInfo_checkBox->isChecked())     m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_INFO);
    if (!ui->showWarning_checkBox->isChecked())  m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_WARNING);
    if (!ui->showCritical_checkBox->isChecked()) m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_CRITICAL);
    if (!ui->showFatal_checkBox->isChecked())    m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_FATAL);

    if (!ui->showCout_checkBox->isChecked())     m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_STDOUT);
    if (!ui->showCerr_checkBox->isChecked())     m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_STDERR);

    if (!ui->showUnknown_checkBox->isChecked())  m_logTypeFilter.push_back(Logging::LogType::LOG_TYPE_UNKNOWN);

    fillMessageList();
}

void MainWindow::updateLogTableContents()
{
    const QString choosenDate = ui->sessions_comboBox->currentText();
    QString dateText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());
    while (m_loggerCore.setNextDate())
    {
        dateText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());

        if (dateText == choosenDate)
        {
            fillMessageList();
            return;
        }
    }

    while (m_loggerCore.setPrevDate())
    {
        dateText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());

        if (dateText == choosenDate)
        {
            fillMessageList();
            return;
        }
    }
}

void MainWindow::setupSignals()
{
    connect(ui->chooseLogFile_pushButton, &QPushButton::clicked, this, &MainWindow::processFile);
    connect(ui->logFileName_lineEdit, &QLineEdit::returnPressed, this, &MainWindow::processFile);

    connect(ui->acceptShowSettings_pushButton, &QPushButton::clicked, this, &MainWindow::acceptViewChanges);
    connect(ui->sessions_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateLogTableContents);
}

void MainWindow::showStatus(const QString &statusText)
{
    ui->status_label->setText(statusText);
}

void MainWindow::fillSessionList()
{
    m_pLoglistModel->clear();
    ui->sessions_comboBox->clear();

    if (m_loggerCore.logDateCount() < 1)
    {
        log("No sessions found in a file");
        return;
    }

    const QString itemText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());
    ui->sessions_comboBox->addItem(itemText);

    while (m_loggerCore.setNextDate())
    {
        const QString itemText = QString("%1 %2").arg(m_loggerCore.time(), m_loggerCore.date());
        ui->sessions_comboBox->addItem(itemText);
    }
    log("Sessions loaded");
}

void MainWindow::fillMessageList()
{
    m_pLoglistModel->clear();

    // Setup header
    QStandardItem* logRow = new QStandardItem("Test");
    QList<QStandardItem*> columns;
    columns.push_back(new QStandardItem("Timestamp"));
    columns.push_back(new QStandardItem("Log type"));
    columns.push_back(new QStandardItem("File"));
    columns.push_back(new QStandardItem("Function"));
    columns.push_back(new QStandardItem("Text"));

    m_pLoglistModel->appendRow(columns);

    auto currentMessage = m_loggerCore.message();
    if (!currentMessage.use_count())
        return;

    while (m_loggerCore.setPrevMessage()); // Reset to first one

    while (m_loggerCore.setNextMessage())
    {
        if (m_logTypeFilter.contains(currentMessage->type)) // Skip anything in filter
            continue;

        columns.clear();

        columns.push_back(new QStandardItem(currentMessage->timestamp));
        columns.push_back(new QStandardItem(Logging::LogMessageStruct::typeString(currentMessage->type)));
        columns.push_back(new QStandardItem(currentMessage->filestamp));
        columns.push_back(new QStandardItem(currentMessage->functionstamp));
        columns.push_back(new QStandardItem(currentMessage->text));
        m_pLoglistModel->appendRow(columns);

        currentMessage = m_loggerCore.message();
    }

    ui->logs_tableView->resizeColumnsToContents();
    auto header = ui->logs_tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    ui->logs_tableView->update();

    log("Logs loaded");
}
