#pragma once
#include "actor.h"
#include <QPointF>
#include <QVector>
#include <QPixmap>

class Enemy : public Actor
{
public:
    int getDamage() const { return damage; }
    Enemy(QPointF start, QPointF end, qreal speed, int dmg, QGraphicsItem *parent = nullptr);

    virtual void updateEntity(qreal dt) override;

private:
    int damage;
    int startFrame;
    int endFrame;
    QPointF pointA;
    QPointF pointB;
    QPointF target;     // hacia dónde se mueve ahora
    qreal m_speed;

    QVector<QPixmap> frames;
    int currentFrame;
    float animTimer;   // acumulador para animar cada X segundos

    void updateDirectionAnimation();
    void updateWalkAnimation(float dt);
};
