#pragma once
#include "entity.h"
#include <QPointF>

class Actor : public Entity
{
public:
    Actor(QGraphicsItem *parent = nullptr);
    virtual ~Actor() override = default;

    virtual void updateEntity(qreal dt) override;

    void setVelocity(const QPointF &v) { velocity = v; }
    QPointF getVelocity() const { return velocity; }

    void setHealth(int h) { health = h; }
    int getHealth() const { return health; }

protected:
    QPointF velocity;
    int health;
};
