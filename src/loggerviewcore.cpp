#include "loggerviewcore.h"

#include <QFile>
#include <QFileInfo>

#include <QDebug>

namespace Logging
{

// Storage for every launch log messages
struct SessionLogStruct
{
    QString date;
    QString time;

    std::vector<std::shared_ptr<LogMessageStruct>> messages;
};

struct LoggerViewCore::LoggerViewCorePrivate
{
    SessionLogStruct m_currentSessionLog;
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
    return d->m_currentSessionLog.date;
}

size_t LoggerViewCore::messageCount() const
{
    return d->m_currentSessionLog.messages.size();
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
    return d->m_currentSessionLog.messages[d->m_currentMessageIndex];
}

std::shared_ptr<LogMessageStruct> LoggerViewCore::message()
{
    return d->m_currentSessionLog.messages[d->m_currentMessageIndex];
}

std::shared_ptr<LogMessageStruct> LoggerViewCore::nextMessage()
{
    if (d->m_currentMessageIndex >= d->m_currentSessionLog.messages.size())
        return {};

    d->m_currentMessageIndex++;
    return d->m_currentSessionLog.messages[d->m_currentMessageIndex];
}

}
