#include "boomerangprojectile.h"
#include "boss3.h"
#include "player3.h"

#include <QtMath>
#include <QGraphicsScene>

static const qreal PI = 3.1416;

BoomerangProjectile::BoomerangProjectile(Boss3 *owner,
                                         Player3 *target,
                                         const QPixmap &sprite,
                                         QGraphicsItem *parent)
    : Actor(parent),
    m_owner(owner),
    m_player(target),
    knockbackActive(false),  // Inicializamos el knockback como inactivo
    knockbackDuration(0.5)   // Duración del knockback en segundos
{
    // Sprite
    if (!sprite.isNull()) {
        setPixmap(sprite);
        setOffset(-pixmap().width() / 2.0, -pixmap().height() / 2.0);
    }

    if (m_owner) {
        m_origin = m_owner->pos();
    }
    if (m_player) {
        m_target = m_player->pos();
    } else {
        m_target = m_origin + QPointF(200, 0);
    }

    // Vector perpendicular para la curvatura (perp a (target - origin))
    QPointF dir = m_target - m_origin;
    qreal len = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
    if (len > 1e-3) {
        dir /= len;
        m_perp = QPointF(-dir.y(), dir.x());  // rotado 90°
        m_arcHeight = len * 0.9;
    } else {
        m_perp = QPointF(0, -1);              // por defecto hacia arriba
    }

    setPos(m_origin);
}

void BoomerangProjectile::updateEntity(qreal dt)
{
    if (m_finished)
        return;

    m_t += dt / m_duration;
    if (m_t > 1.0)
        m_t = 1.0;

    qreal s;
    if (m_t < 0.5)
        s = m_t * 2.0;
    else
        s = (1.0 - m_t) * 2.0;

    QPointF dir  = m_target - m_origin;
    QPointF base = m_origin + dir * s;

    qreal sinVal = qSin(2.0 * PI * m_t);      // 0 → +1 → 0 → -1 → 0
    QPointF offset = m_perp * (m_arcHeight * sinVal);

    QPointF newPos = base + offset;
    setPos(newPos);

    m_angle += m_rotSpeed * dt;
    setRotation(m_angle);

    // --- Colisión con el jugador ---
    if (!m_alreadyHitPlayer && m_player && m_player->scene() && collidesWithItem(m_player)) {
        m_alreadyHitPlayer = true;

        // Si el jugador está en guardia, no le hacemos daño ni empuje
        if (!m_player->isGuarding()) {
            int hp = m_player->health();
            hp -= m_damage;
            if (hp < 0) hp = 0;
            m_player->setHealth(hp);

            if (m_owner) {
                m_owner->registerPlayerHit(Boss3::Boomerang);
            }

            // Activar knockback cuando el boomerang golpea al jugador
            knockbackActive = true;
            knockbackDuration = 0.25;
        }
    }

    // --- Vuelta al boss completada ---
    if (m_t >= 1.0) {
        m_finished = true;
        if (m_owner) {
            m_owner->onBoomerangReturned(this);
        }
    }

    // ** EMPUJAR AL JUGADOR DURANTE EL KNOCKBACK **
    if (knockbackActive && m_player) {
        if (m_player->scene()) {
            if (knockbackDuration > 0) {
                qreal knockbackSpeed = 15.0;
                QPointF knockbackDir = m_player->pos() - m_owner->pos();
                qreal len = qSqrt(knockbackDir.x() * knockbackDir.x() + knockbackDir.y() * knockbackDir.y());
                if (len > 0) {
                    knockbackDir /= len;
                    QPointF push = knockbackDir * knockbackSpeed;

                    QPointF pos = m_player->pos();
                    pos += push;
                    pos.setY(pos.y() - 30.0);
                    m_player->setPos(pos);

                    knockbackDuration -= dt;
                }
            } else {
                // Cuando el knockback termina
                knockbackActive = false;
            }
        } else {
            // Si el jugador no está en la escena, desactivar el knockback
            knockbackActive = false;
        }
    }
}
