#include "logcolordelegate.h"

#include <QPainter>
#include <QDebug>

#include "loggerviewcore.h"

LogColorDelegate::LogColorDelegate(QObject *parent) :
    QStyledItemDelegate{parent}
{

}

LogColorDelegate::~LogColorDelegate()
{

}

void LogColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if ((index.column() != 1) || (index.row() == 0))
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    auto text = index.data().toString();
    auto textType = Logging::LogMessageStruct::typeFromString(text);

    auto posRect = option.rect;

    auto w = posRect.width();
    posRect.setX(posRect.x() + w / 4);
    posRect.setWidth(w - w / 2);

    auto painterPen = painter->pen();

    switch (textType)
    {
    case Logging::LogType::LOG_TYPE_DEBUG:
        painterPen.setColor(QColor(50, 50, 250));
        break;

    case Logging::LogType::LOG_TYPE_INFO:
        painterPen.setColor(QColor(100, 100, 100));
        break;

    case Logging::LogType::LOG_TYPE_WARNING:
        painterPen.setColor(QColor(150, 150, 50));
        break;

    case Logging::LogType::LOG_TYPE_CRITICAL:
        painterPen.setColor(QColor(250, 50, 50));
        break;

    case Logging::LogType::LOG_TYPE_FATAL:
        painterPen.setColor(QColor(250, 50, 250));
        break;

    case Logging::LogType::LOG_TYPE_STDOUT:
        painterPen.setColor(QColor(50, 50, 50));
        break;

    case Logging::LogType::LOG_TYPE_STDERR:
        painterPen.setColor(QColor(200, 70, 70));
        break;

    case Logging::LogType::LOG_TYPE_UNKNOWN:
        painterPen.setColor(QColor(30, 30, 30));
        break;
    }

    painterPen.setWidth(2);
    painter->setPen(painterPen);

    painter->drawRect(posRect);

    QTextOption posOption;
    posOption.setAlignment(Qt::AlignCenter);
    painter->drawText(posRect, text, posOption);
}
