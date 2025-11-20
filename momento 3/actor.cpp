#include "actor.h"

Actor::Actor(QGraphicsItem *parent)
    : Entity(parent), velocity(0,0), health(100)
{
}

void Actor::updateEntity(qreal dt)
{
    // default simple movement: translate by velocity * dt
    if (!velocity.isNull()) {
        moveBy(velocity.x() * dt, velocity.y() * dt);
    }
}
