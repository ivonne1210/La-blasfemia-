#ifndef ENEMY2_H
#define ENEMY2_H

#include "enemy.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>

class Enemy2 : public Enemy
{
public:
    Enemy2(QPointF pos, int health = 50);
    virtual ~Enemy2() {}
    void updateEntity(qreal dt) override;

    int getHealth() const { return health_; }
    void takeDamage(int dmg);

    void enableMovement(bool enable) { canMove = enable; }

private:
    int health_;
    QPixmap pix;
    bool canMove = false;
    float speed = 100;
};

#endif // ENEMY2_H
