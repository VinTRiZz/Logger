#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "logger.h"

#include <QMainWindow>

#include <QStandardItemModel>

#include "loggerviewcore.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void processFile();

    void acceptViewChanges();

    void updateLogTableContents();

private:
    Ui::MainWindow *ui;

    Logging::LoggerViewCore m_loggerCore;

    QStandardItemModel * m_pLoglistModel;

    void setupSignals();

    void fillSessionList();
    void fillMessageList();
};

#endif // MAINWINDOW_H
