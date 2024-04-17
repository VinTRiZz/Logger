#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "logger.h"

#include <QMainWindow>

#include <QStandardItemModel>

#include "loggerviewcore.h"

#include <QVector>

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

    void chooseFileFromFilesystem();

private:
    Ui::MainWindow *ui;

    Logging::LoggerViewCore m_loggerCore;
    bool m_showAsFullPath {true};

    QVector<Logging::LogType> m_logTypeFilter {Logging::LogType::LOG_TYPE_UNKNOWN};

    QStandardItemModel * m_pLoglistModel;

    void setupSignals();

    void showStatus(const QString& statusText);

    void fillSessionList();
    void fillMessageList();
};

#endif // MAINWINDOW_H
