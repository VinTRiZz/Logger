#ifndef LOGCOLORDELEGATE_H
#define LOGCOLORDELEGATE_H

#include <QStyledItemDelegate>

class LogColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    LogColorDelegate(QObject* parent = 0);
    ~LogColorDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // LOGCOLORDELEGATE_H
