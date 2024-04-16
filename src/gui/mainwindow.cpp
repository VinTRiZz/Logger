#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#define log(what) ui->status_label->setText(what);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    if (m_loggerCore.parseFile(filename))
    {
        qDebug() << "Parsing complete";
        log("Parsing complete");
    }
    else
    {
        log("File invalid or not exist");
    }
}

void MainWindow::setupSignals()
{
    connect(ui->chooseLogFile_pushButton, &QPushButton::clicked, this, &MainWindow::processFile);
}
