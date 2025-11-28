#include "enemy.h"
#include <QtMath>

Enemy::Enemy(QPointF start, QPointF end, qreal speed, int dmg, QGraphicsItem *parent)
    : Actor(parent),
    pointA(start),
    pointB(end),
    target(end),
    m_speed(speed),
    damage(dmg),
    currentFrame(0),
    animTimer(0)
{
    // Cargar sprites
    for (int i = 0; i < 12; i++) {
        QString path =
            QString("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/enemy%1.png").arg(i);
        QPixmap frame(path);
        frame = frame.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        frames.push_back(frame);
    }

    setPixmap(frames[0]);
    setPos(start);
    startFrame = 3;
    endFrame = 5;
}

void Enemy::updateEntity(qreal dt)
{
    QPointF pos = this->pos();
    QPointF dir = (target - pos);

    qreal dist = qSqrt(dir.x()*dir.x() + dir.y()*dir.y());

    if (dist < 3) {
        target = (target == pointA ? pointB : pointA);
        return;
    }

    dir /= dist;

    setVelocity(QPointF(dir.x() * m_speed, dir.y() * m_speed));

    Actor::updateEntity(dt);

    updateDirectionAnimation();
    updateWalkAnimation(dt);
}

void Enemy::updateDirectionAnimation()
{
    QPointF v = getVelocity();

    if (qAbs(v.y()) > qAbs(v.x())) {
        if (v.y() > 0) { startFrame = 3; endFrame = 5; }   // Down
        else            { startFrame = 0; endFrame = 2; }   // Up
    } else {
        if (v.x() > 0) { startFrame = 9; endFrame = 11; }   // Right
        else           { startFrame = 6; endFrame = 8; }    // Left
    }

    if (currentFrame < startFrame || currentFrame > endFrame)
        currentFrame = startFrame;
}


void Enemy::updateWalkAnimation(float dt)
{
    animTimer += dt;

    if (animTimer > 0.15f) {
        animTimer = 0.0f;

        currentFrame++;
        if (currentFrame > endFrame)
            currentFrame = startFrame;
    }

    setPixmap(frames[currentFrame]);
    setOffset(-pixmap().width()/2, -pixmap().height()/2);
}

