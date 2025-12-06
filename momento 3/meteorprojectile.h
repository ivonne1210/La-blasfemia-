#ifndef METEORPROJECTILE_H
#define METEORPROJECTILE_H

#include "actor.h"

class Boss3;
class Player3;

class MeteorProjectile : public Actor
{
public:
    MeteorProjectile(Boss3 *owner,
                     Player3 *player,
                     const QPixmap &sprite,
                     QGraphicsItem *parent = nullptr);

    void updateEntity(qreal dt) override;

    bool isFinished() const { return m_finished; }
    bool knockbackActive;
    qreal knockbackDuration;

private:
    Boss3   *m_owner  = nullptr;
    Player3 *m_player = nullptr;

    qreal vy       = 0.0;
    qreal gravity  = 1600.0;   // gravedad más fuerte que la del player para que caiga rápido
    bool  m_finished         = false;
    bool  m_alreadyHitPlayer = false;
    bool m_isCollidingWithPlayer;

    int   m_damage = 20;       // daño por meteorito
};

#endif // METEORPROJECTILE_H
