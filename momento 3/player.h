#pragma once
#include "actor.h"
#include <QTimer>
#include <vector>
#include <QPixmap>

class Player : public Actor
{
public:
    Player(QGraphicsItem *parent = nullptr);

    void updateEntity(qreal dt) override;

    void moveLeft(bool on);
    void moveRight(bool on);
    void moveUp(bool on);
    void moveDown(bool on);

    int speed() const { return m_speed; }

    int health() const { return m_health; }
    void setHealth(int h) { m_health = h; }
    void takeDamage(int amount);
    void storeLastSafePos();               // guardar posición previa
    void restoreLastSafePos();


private:
    bool left, right, up, down;
    int m_speed;
    int m_health;

    std::vector<QPixmap> walkFrames;
    int currentFrame;
    QTimer *animTimer;

    void updateAnimation();
    float invincibleTime;
    float blinkTimer;
    bool blinking;
    QPointF lastSafePos;
};
