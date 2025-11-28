#ifndef PLAYER2_H
#define PLAYER2_H

#pragma once
#include "actor.h"
#include <QTimer>
#include <vector>
#include <QPixmap>

class Player2 : public Actor
{
public:
    Player2(QGraphicsItem *parent = nullptr);

    void updateEntity(qreal dt) override;

    void moveLeft(bool on);
    void moveRight(bool on);
    void moveUp(bool on);
    void moveDown(bool on);
    void defense(bool on);
    void atack(bool on);

    int speed() const { return m_speed; }

    int health() const { return m_health; }
    void setHealth(int h) { m_health = h; }
    void takeDamage(int amount);
    void storeLastSafePos();
    void restoreLastSafePos();
    bool left, right, up, down, defen, atc;
    std::vector<QPixmap> walkFrames;
    int currentFrame;
    QTimer *animTimer;
    int m_speed;

private:
    int m_health;

    void updateAnimation();
    float invincibleTime;
    float blinkTimer;
    bool blinking;
    QPointF lastSafePos;
};

#endif // PLAYER2_H
