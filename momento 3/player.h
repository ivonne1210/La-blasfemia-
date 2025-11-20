#pragma once
#include "actor.h"
#include <QTimer>
#include <vector>
#include <QPixmap>

class Player : public Actor
{
public:
    Player(QGraphicsItem *parent = nullptr);
    ~Player() override = default;

    void updateEntity(qreal dt) override;

    void moveLeft(bool on);
    void moveRight(bool on);
    void moveUp(bool on);
    void moveDown(bool on);

    int speed() const { return m_speed; }

private:
    bool left, right, up, down;
    int m_speed;

    std::vector<QPixmap> walkFrames;
    int currentFrame;
    QTimer *animTimer;

    void updateAnimation();
};
