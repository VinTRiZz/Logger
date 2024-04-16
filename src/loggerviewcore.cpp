#include "loggerviewcore.h"

#include <QFile>
#include <QFileInfo>

#include <QRegularExpression>

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

    constexpr int64_t bufferSize = 1024 * 1024;
    char dataBuffer[bufferSize];
    memset(dataBuffer, '\0', bufferSize);

    while (d->m_logFile.readLine(dataBuffer, bufferSize) > 0)
    {
        parseLine(dataBuffer);
    }

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

void LoggerViewCore::parseLine(const QString &lineData)
{
    if (!lineData.contains(QRegularExpression("\[[0-9]{2}.[0-9]{2}.[0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2}\]"))) //  \[\(DEBUG\)|\(WARNING\)|\(INFO\)\]
    {
//        qDebug() << "Not contains needed data:" << lineData;
        return;
    }

    QRegularExpression dateExp("[0-9]{2}.[0-9]{2}.[0-9]{4}");
    QRegularExpression timeExp("[0-9]{2}:[0-9]{2}:[0-9]{2}");

    qDebug() << "Contains data:" << lineData;
    if (lineData.contains("Launch time"))
    {
        auto match = dateExp.match(lineData);
        if (match.hasMatch())
            d->m_currentSessionLog.date = match.captured(0);

        match = timeExp.match(lineData);
        if (match.hasMatch())
            d->m_currentSessionLog.time = match.captured(0);

        qDebug() << "Date and time parsed:" << d->m_currentSessionLog.date << d->m_currentSessionLog.time;
        return;
    }

    qDebug() << "Not a date or time";
}

}
