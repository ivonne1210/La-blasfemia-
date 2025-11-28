#ifndef ARROW_H
#define ARROW_H

#include "entity.h"
#include <QGraphicsPixmapItem>

class Arrow : public Entity
{
public:
    Arrow(QPointF startPos, int dir, float speed, int dmg);

    void updateEntity(qreal dt) override;
    int direction;

private:
    float velocity;
    int damage;

    QPixmap spriteUp;
    QPixmap spriteDown;
};

#endif
