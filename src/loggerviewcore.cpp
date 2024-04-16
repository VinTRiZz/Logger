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

    qDebug() << "Found" << d->m_currentSessionLog.messages.size() << "log messages";

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
    if (!lineData.contains(QRegularExpression("\\[[0-9]{2}.[0-9]{2}.[0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2}\\]")))
    {
//        qDebug() << "Skipping line:" << lineData;
        return;
    }
//    qDebug() << "Parsing line:" << lineData;

    QRegularExpression dateExp("[0-9]{2}.[0-9]{2}.[0-9]{4}");
    QRegularExpression timeExp("[0-9]{2}:[0-9]{2}:[0-9]{2}");

    if (lineData.contains("Launch time"))
    {
        auto match = dateExp.match(lineData);
        if (match.hasMatch())
            d->m_currentSessionLog.date = match.captured(0);

        match = timeExp.match(lineData);
        if (match.hasMatch())
            d->m_currentSessionLog.time = match.captured(0);

//        qDebug() << "Found session:" << d->m_currentSessionLog.date << d->m_currentSessionLog.time;
        return;
    }

    std::shared_ptr<LogMessageStruct> currentLogMessage = std::shared_ptr<LogMessageStruct>(new LogMessageStruct(), std::default_delete<LogMessageStruct>());

    QRegularExpression timestampMatch(QString("%1 %2").arg(dateExp.pattern(), timeExp.pattern()));
    auto match = timestampMatch.match(lineData);
    if (!match.hasMatch())
        return;

    currentLogMessage->timestamp = match.captured(0);
//    qDebug() << "Timestamp:" << currentLogMessage->timestamp;

    QRegularExpression filestampMatch("([\-\_\.A-Za-z0-9]+[/]{0,1}){1,} : [0-9]+");
    match = filestampMatch.match(lineData);
    if (!match.hasMatch())
        return;

    currentLogMessage->filestamp = match.captured(0);
//    qDebug() << "File:" << currentLogMessage->filestamp;

    QRegularExpression logTypeMatch("\\[(DEBUG|INFO|WARNING|CRITICAL|FATAL)\\]");
    match = logTypeMatch.match(lineData);
    if (!match.hasMatch())
        return;

    QString type = match.captured(0);
    if (type.size() < 2)
        return;

    type.remove(0, 1);

    switch (type[0].toLatin1())
    {
    case 'D':
//        qDebug() << "Debug!";
        currentLogMessage->type = LogType::LOG_TYPE_DEBUG;
        break;

    case 'I':
//        qDebug() << "Info!";
        currentLogMessage->type = LogType::LOG_TYPE_INFO;
        break;

    case 'W':
//        qDebug() << "Warning!";
        currentLogMessage->type = LogType::LOG_TYPE_WARNING;
        break;

    case 'C':
//        qDebug() << "Critical!";
        currentLogMessage->type = LogType::LOG_TYPE_CRITICAL;
        break;

    case 'F':
//        qDebug() << "Fatal!";
        currentLogMessage->type = LogType::LOG_TYPE_FATAL;
        break;

    default:
        qDebug() << "Unknown log type:" << type;
        return;
    }

    const QString wordRegExp("[:]{0,2}[A-Za-z0-9&*<>]+");
    const QString argRegExp = QString("(%1){1,}( %1){0,}(\\, ){0,1}").arg(wordRegExp);
    QRegularExpression functionMatch(QString("(%1 ){1,}(%1)+\\((%2){0,}\\)").arg(wordRegExp, argRegExp));
    match = functionMatch.match(lineData);
    if (!match.hasMatch())
    {
        qDebug() << "No function here:" << lineData;
        return;
    }

    currentLogMessage->functionstamp = match.captured(0);
//    qDebug() << "Function is:" << currentLogMessage->functionstamp;

//    qDebug() << "Found log:"
//             << currentLogMessage->timestamp
//             << currentLogMessage->filestamp
//             << currentLogMessage->functionstamp
//             << "T:" << currentLogMessage->type
//             << currentLogMessage->text
//    ;

    d->m_currentSessionLog.messages.push_back(currentLogMessage);
}

}
