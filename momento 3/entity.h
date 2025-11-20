#pragma once
#include <QGraphicsPixmapItem>
#include <QRectF>

class Entity : public QGraphicsPixmapItem
{
public:
    enum { Type = UserType + 1 };
    Entity(QGraphicsItem *parent = nullptr);
    virtual ~Entity() override = default;

    virtual void updateEntity(qreal dt) = 0;
    virtual QRectF boundingBox() const;
    int type() const override { return Type; }
};
