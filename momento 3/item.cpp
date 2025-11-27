#include "item.h"
#include <QPainter>
#include <QPropertyAnimation>

Item::Item(const QString& spritePath, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent)
{
    QPixmap original(spritePath);

    // 🚀 Escala por ejemplo al 150% del tamaño original
    QPixmap scaled = original.scaled(
        original.width() * 1.5,
        original.height() * 1.5,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    setPixmap(scaled);
    setZValue(5);
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


