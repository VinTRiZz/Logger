#ifndef LOGGERVIEWCORE_H
#define LOGGERVIEWCORE_H

#include <memory>

#include <QString>
#include <QVector>

namespace Logging
{

enum LogType
{
    LOG_TYPE_UNKNOWN,

    // Qt logs
    LOG_TYPE_DEBUG,
    LOG_TYPE_INFO,
    LOG_TYPE_WARNING,
    LOG_TYPE_CRITICAL,
    LOG_TYPE_FATAL,

    // Std
    LOG_TYPE_STDOUT,
    LOG_TYPE_STDERR
};

// Storage for messages
struct LogMessageStruct
{
    QString text;
    LogType type;

    QString filename;
    QString timestamp;
};


class LoggerViewCore
{
public:
    LoggerViewCore();
    ~LoggerViewCore();

    // Setup log file name or get it
    void setLogFile(const QString& filename);
    QString logFileName() const;

    // First one parses set before file, second one parses inserted
    bool parseFile();
    bool parseFile(const QString& filename);

    // Date and time program started in current log period
    QString date();
    QString time();

    // Switch between program launches
    bool prevDate();
    bool nextDate();

    // Count of messages parsed
    size_t messageCount() const;
    size_t currentMessageIndex() const;

    // Iterate between messages. Unsafe (take care of indexes)
    std::shared_ptr<LogMessageStruct> prevMessage();    // Get past and index -1
    std::shared_ptr<LogMessageStruct> message();        // Current
    std::shared_ptr<LogMessageStruct> nextMessage();    // Get next and index +1

private:
    struct LoggerViewCorePrivate;
    std::unique_ptr<LoggerViewCorePrivate> d;

    void parseLine(const QString& lineData);
};

}
#endif // LOGGERVIEWCORE_H
