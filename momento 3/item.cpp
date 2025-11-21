#include "item.h"
#include <QPainter>
#include <QPropertyAnimation>

Item::Item(const QString& spritePath, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent)
{
    setPixmap(QPixmap(spritePath));
    setZValue(5); // encimado del piso

    // activar colisiones
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

QRectF Item::boundingRect() const
{
    return QGraphicsPixmapItem::boundingRect();
}

QPainterPath Item::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}


