#ifndef BOOMERANGPROJECTILE_H
#define BOOMERANGPROJECTILE_H

#include "actor.h"
#include <QPointF>
#include <QPixmap>

class Boss3;
class Player3;

class BoomerangProjectile : public Actor
{
public:
    BoomerangProjectile(Boss3 *owner,
                        Player3 *target,
                        const QPixmap &sprite,
                        QGraphicsItem *parent = nullptr);

    void updateEntity(qreal dt) override;

    bool finished() const { return m_finished; }

private:
    Boss3   *m_owner  = nullptr;
    Player3 *m_player = nullptr;

    QPointF m_origin;   // donde estaba el boss al disparar
    QPointF m_target;   // donde estaba el jugador al disparar
    QPointF m_perp;     // vector perpendicular para la curvatura

    qreal   m_t         = 0.0;    // parámetro 0..1 (ida+vuelta)
    qreal   m_duration  = 1.5;    // tiempo total en segundos
    qreal   m_arcHeight = 120.0;  // altura de la curva
    qreal   m_angle     = 0.0;    // rotación visual
    qreal   m_rotSpeed  = 720.0;  // grados/segundo

    bool    m_finished          = false;
    bool    m_alreadyHitPlayer  = false;
    bool   m_deflected        = false;
    int     m_damage            = 15;

    bool knockbackActive;
    qreal knockbackDuration;
};

#endif // BOOMERANGPROJECTILE_H
