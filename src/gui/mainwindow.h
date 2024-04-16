#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "logger.h"

#include <QMainWindow>


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

private:
    Ui::MainWindow *ui;

    Logging::LoggerViewCore m_loggerCore;

    void setupSignals();
};

#endif // MAINWINDOW_H
