#include "meteorprojectile.h"
#include "boss3.h"
#include "player3.h"
#include <QGraphicsScene>

MeteorProjectile::MeteorProjectile(Boss3 *owner,
                                   Player3 *player,
                                   const QPixmap &sprite,
                                   QGraphicsItem *parent)
    : Actor(parent),
    m_owner(owner),
    m_player(player),
    m_isCollidingWithPlayer(false) // Agregamos un estado para saber si el meteorito impactó al jugador
{
    if (!sprite.isNull()) {
        setPixmap(sprite);
        setOffset(-pixmap().width() / 2.0, -pixmap().height() / 2.0);
    }
}

void MeteorProjectile::updateEntity(qreal dt)
{
    if (m_finished)
        return;

    vy += gravity * dt;

    QPointF p = pos();
    p.setY(p.y() + vy * dt);
    setPos(p);

    // **Colisión inelástica**: Si el meteorito impacta con el jugador
    if (!m_isCollidingWithPlayer && m_player && collidesWithItem(m_player)) {
        m_isCollidingWithPlayer = true;

        if (!m_player->isGuarding()) {
            int hp = m_player->health();
            QPointF playerPos = m_player->pos();
            m_player->setPos(playerPos.x()-30, p.y()-30.0);
            hp -= m_damage;
            if (hp < 0) hp = 0;
            m_player->setHealth(hp);

            if (m_owner) {
                m_owner->registerPlayerHit(Boss3::MeteorRain);
            }
        }
    }

    // Si el meteorito ha caído por completo (fuera de la escena)
    if (scene()) {
        QRectF r = scene()->sceneRect();
        if (p.y() > r.bottom() + 50.0) {
            m_finished = true;
        }
    }
}
