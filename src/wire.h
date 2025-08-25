#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsLineItem>
#include <QPoint>
#include "node.h"

class Wire : public QGraphicsLineItem
{
private:
    Node node1;
    Node node2;

public:
    Wire();
    Wire(const QPoint& p1, const QPoint& p2);
    ~Wire() override;
};

#endif // WIRE_H
