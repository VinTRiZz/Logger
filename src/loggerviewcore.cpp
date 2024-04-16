#include "loggerviewcore.h"

#include <QFile>
#include <QFileInfo>

#include <QDebug>

namespace Logging
{

struct LogMessagePeriodStruct
{
    QString date;
    QString time;

    std::vector<std::shared_ptr<LogMessageStruct>> messages;
};

struct LoggerViewCore::LoggerViewCorePrivate
{
    LogMessagePeriodStruct m_currentLog;
    size_t m_currentMessageIndex {0};

    QFile m_logFile;
    quint64 m_currentFilePos;
};

LoggerViewCore::LoggerViewCore() :
    d {new LoggerViewCorePrivate}
{

}

LoggerViewCore::~LoggerViewCore()
{

}

void LoggerViewCore::setLogFile(const QString &filename)
{
    if (d->m_logFile.isOpen())
        d->m_logFile.close();

    d->m_logFile.setFileName(filename);
}

QString LoggerViewCore::logFileName() const
{
    return d->m_logFile.fileName();
}

bool LoggerViewCore::parseFile()
{
    if (!d->m_logFile.exists() || QFileInfo(d->m_logFile.fileName()).isDir())
    {
        qDebug() << "Invalid filename:" << d->m_logFile.fileName();
        return false;
    }

    d->m_logFile.open(QIODevice::ReadOnly);

    return true;
}

bool LoggerViewCore::parseFile(const QString &filename)
{
    this->setLogFile(filename);
    return this->parseFile();
}

QString LoggerViewCore::date()
{
    return d->m_currentLog.date;
}

size_t LoggerViewCore::messageCount() const
{
    return d->m_currentLog.messages.size();
}

size_t LoggerViewCore::currentMessageIndex() const
{
    return d->m_currentMessageIndex;
}

std::shared_ptr<LogMessageStruct> LoggerViewCore::prevMessage()
{
    if (d->m_currentMessageIndex == 0)
        return {};

    d->m_currentMessageIndex--;
    return d->m_currentLog.messages[d->m_currentMessageIndex];
}

std::shared_ptr<LogMessageStruct> LoggerViewCore::message()
{
    return d->m_currentLog.messages[d->m_currentMessageIndex];
}

std::shared_ptr<LogMessageStruct> LoggerViewCore::nextMessage()
{
    if (d->m_currentMessageIndex >= d->m_currentLog.messages.size())
        return {};

    d->m_currentMessageIndex++;
    return d->m_currentLog.messages[d->m_currentMessageIndex];
}

}
