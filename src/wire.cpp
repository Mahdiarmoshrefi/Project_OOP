#include "wire.h"
#include <QLineF>
#include <QPointF>

Wire::Wire()
    : QGraphicsLineItem()
{
}

Wire::Wire(const QPoint& p1, const QPoint& p2)
    : QGraphicsLineItem()
{
    // تبدیل QPoint به QPointF و ست‌کردن خط
    setLine(QLineF(QPointF(p1), QPointF(p2)));
}

Wire::~Wire() = default;
