#ifndef ITEM_H
#define ITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsItem>

class Item : public QGraphicsPixmapItem
{
public:
    Item(const QString& spritePath, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
};

#endif // ITEM_H
