#include "entity.h"

Entity::Entity(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
}

QRectF Entity::boundingBox() const
{
    return boundingRect();
}
