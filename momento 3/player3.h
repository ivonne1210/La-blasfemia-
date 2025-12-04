#ifndef PLAYER3_H
#define PLAYER3_H

#include "actor.h"      // La misma base que usan Enemy / Player2
#include <QVector>
#include <QPixmap>

class Player3 : public Actor
{
public:
    Player3(QGraphicsItem *parent = nullptr);

    void updateEntity(qreal dt) override;
    void moveLeft(bool pressed);
    void moveRight(bool pressed);
    void jump(bool pressed);

    bool isOnGround() const { return onGround; }
    void setGroundY(qreal y) { groundY = y; }
    void setVerticalVelocity(qreal v);

private:
    void updatePhysics(qreal dt);
    void updateAnimation(qreal dt);
    void applyAnimFrame();

    enum AnimState { Idle, Run, JumpState, FallState };

    // Input
    bool leftPressed  = false;
    bool rightPressed = false;
    bool jumpPressed  = false;

    // Física
    qreal vx = 0.0;
    qreal vy = 0.0;
    qreal moveSpeed = 220.0;     // px/s
    qreal jumpSpeed = -650.0;    // px/s (negativo porque va hacia arriba)
    qreal gravity   = 1200.0;    // px/s^2
    qreal groundY   = 780;     // piso por defecto
    bool  onGround  = false;

    // Animación
    AnimState animState   = Idle;
    AnimState prevState   = Idle;
    int       currentFrame = 0;
    qreal     animTimer    = 0.0;
    qreal     frameDuration = 0.10;  // 100 ms por frame

    int facing = 1;  // 1 = derecha, -1 = izquierda

    QVector<QPixmap> idleFrames;
    QVector<QPixmap> runFrames;
    QVector<QPixmap> jumpFrames;
    QVector<QPixmap> fallFrames;
};

#endif // PLAYER3_H
