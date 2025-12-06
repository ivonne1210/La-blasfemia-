#include "boss3.h"
#include "player3.h"
#include "boomerangprojectile.h"
#include "meteorprojectile.h"

#include <QTransform>
#include <QtMath>
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QDebug>

Boss3::Boss3(Player3 *player, QGraphicsItem *parent)
    : Actor(parent),
    playerRef(player)
{
    setZValue(20);

    const QString base = "C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/";
    QVector<QPixmap> idle;
    QVector<QPixmap> boom;
    QVector<QPixmap> meteor;
    QVector<QPixmap> jump;

    QPixmap p(base + "boss1.png");
    if (!p.isNull()) {
        p = p.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        idle.append(p);
    }

    QPixmap b(base + "boss_espada.png");
    if (!b.isNull()) {
        b = b.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        boom.append(b);
    }

    QPixmap c(base + "boss_meteor.png");
    if (!c.isNull()) {
        c = c.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        meteor.append(c);
    }

    QPixmap j(base + "boss_jump.png");
    if (!j.isNull()) {
        j = j.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        jump.append(j);
    }

    boomerangSprite = QPixmap(base + "espada.png");
    if (!boomerangSprite.isNull()) {
        boomerangSprite = boomerangSprite.scaled(90, 90,
                                                 Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        qDebug() << "No se pudo cargar espada.png";
    }

    meteorSprite = QPixmap(base + "meteor.png");   // cambia nombre si usas otro archivo
    if (!meteorSprite.isNull()) {
        meteorSprite = meteorSprite.scaled(90, 90,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
    } else {
        qDebug() << "No se pudo cargar sprite de meteorito";
    }

    setIdleSprites(idle);
    setBoomCastSprites(boom);
    setMeteorCastSprites(meteor);
    setJumpSprites(jump);

    m_maxHealth = 500;
    m_health    = m_maxHealth;

}


void Boss3::setIdleSprites(const QVector<QPixmap> &frames)
{
    idleFrames = frames;
    if (!idleFrames.isEmpty()) {
        setPixmap(idleFrames[0]);
        setOffset(-pixmap().width() / 2.0, -pixmap().height() / 2.0);
    }
}

void Boss3::setCastSprites(const QVector<QPixmap> &frames)
{
    castFrames = frames;
}

void Boss3::setJumpSprites(const QVector<QPixmap> &frames)
{
    jump = frames;
}

void Boss3::updateEntity(qreal dt)
{
    updatePhysics(dt);
    updateState(dt);
    updateAnimation(dt);

    if (boomerang) {
        boomerang->updateEntity(dt);
    }

    for (int i = meteors.size() - 1; i >= 0; --i) {
        MeteorProjectile *m = meteors[i];
        if (!m) {
            meteors.removeAt(i);
            continue;
        }

        m->updateEntity(dt);

        if (m->isFinished()) {
            if (m->scene())
                m->scene()->removeItem(m);
            delete m;
            meteors.removeAt(i);
        }
    }
}

/* ===================== FÍSICA ===================== */

void Boss3::updatePhysics(qreal dt)
{
    // 1) Mirar hacia el jugador SOLO si no está cas­teando la lluvia
    if (playerRef && state != DoMeteorRain) {
        if (playerRef->x() < x())
            facing = -1;
        else
            facing = 1;
    }

    // 2) Si está haciendo la lluvia de meteoritos, se queda quieto en el punto objetivo
    if (state == DoMeteorRain) {
        setPos(meteorTargetX, meteorTargetY);
        vx = 0.0;
        vy = 0.0;
        onGround = false;      // aquí no nos importa el suelo
        return;                // <- muy importante: no seguir con la física normal
    }

    // 3) Para otros estados: gravedad normal (menos cuando va caminando hacia el centro)
    if (state != MoveToMeteorPos) {
        vy += gravity * dt;
    }

    QPointF pos = this->pos();
    pos.setX(pos.x() + vx * dt);
    pos.setY(pos.y() + vy * dt);

    // Limitar X dentro de la escena
    if (scene() && !pixmap().isNull()) {
        QRectF r = scene()->sceneRect();
        qreal halfW = pixmap().width() / 2.0;

        if (pos.x() < r.left() + halfW)
            pos.setX(r.left() + halfW);
        if (pos.x() > r.right() - halfW)
            pos.setX(r.right() - halfW);
    }

    // Suelo solo si NO está en MoveToMeteorPos (cuando va caminando al centro dejamos que vaya "flotando" según tu diseño)
    if (state != MoveToMeteorPos) {
        if (pos.y() >= groundY) {
            pos.setY(groundY);
            vy = 0.0;
            onGround = true;
        } else {
            onGround = false;
        }
    } else {
        onGround = false;
    }

    setPos(pos);
}



/* ===================== "COGNITIVO" ===================== */

void Boss3::registerPlayerHit(AttackType type)
{
    switch (type) {
    case Boomerang:
        statsBoomerang.hits++;
        break;
    case JumpAttack:
        statsJump.hits++;
        break;
    case MeteorRain:
        statsMeteor.hits++;
        break;
    }
}

/* ===================== MÁQUINA DE ESTADOS ===================== */

void Boss3::updateState(qreal dt)
{
    stateTimer += dt;

    switch (state) {
    case Idle:
        if (stateTimer >= attackCooldown) {
            stateTimer = 0.0;
            state = ChoosingAttack;
        }
        break;

    case MoveToMeteorPos: {
        QPointF p = pos();
        qreal dx = meteorTargetX - p.x();
        qreal dy = meteorTargetY - p.y();

        qreal dist = qSqrt(dx*dx + dy*dy);
        const qreal walkSpeed = 200.0;   // velocidad bajita para que se vea el trayecto

        if (dist < 5.0) {
            // Ya llegó: lo encajamos exactamente en el punto objetivo
            setPos(meteorTargetX, meteorTargetY);

            vx = 0.0;
            vy = 0.0;

            meteorActive       = true;
            meteorDuration     = 5.0;
            meteorSpawnTimer   = 0.0;

            state      = DoMeteorRain;
            stateTimer = 0.0;
            animState  = ACast;

            qDebug() << "[Boss] Llegó al centro, comienza lluvia de meteoritos";
        } else {
            // Normalizar vector (dx,dy) y desplazarse en esa dirección
            qreal nx = dx / dist;
            qreal ny = dy / dist;

            vx = nx * walkSpeed;
            vy = ny * walkSpeed;

            facing = (dx > 0) ? 1 : -1;  // para voltear el sprite según hacia dónde camina
        }
        break;
    }


    case ChoosingAttack:
        startNextAttack();
        break;

    case DoBoomerang:
        if (!boomerangActive) {
            finishAttack();
        }
        break;

    case DoJump:
        if (onGround && stateTimer > 0.3) {
            finishAttack();
        }
        break;

    case DoMeteorRain:
        if (!meteorActive) {
            finishAttack();
            break;
        }

        meteorDuration   -= dt;
        meteorSpawnTimer += dt;

        if (scene() && meteorSpawnTimer >= meteorSpawnInterval) {
            meteorSpawnTimer = 0.0;

            QRectF r = scene()->sceneRect();
            qreal margin = 40.0;
            qreal xMin = r.left()  + margin;
            qreal xMax = r.right() - margin;

            // X aleatoria en todo el ancho del mapa
            // xMin y xMax son qreal, pero bounded quiere enteros
            int randX = QRandomGenerator::global()->bounded(
                static_cast<int>(xMin),
                static_cast<int>(xMax)
                );
            qreal x = static_cast<qreal>(randX);

            // Nacen un poquito por encima de la pantalla
            qreal y = r.top() - 80.0;

            QPixmap sprite = meteorSprite;
            if (sprite.isNull() && !meteorCastFrames.isEmpty())
                sprite = meteorCastFrames[0];
            else if (sprite.isNull() && !idleFrames.isEmpty())
                sprite = idleFrames[0];

            auto *m = new MeteorProjectile(this, playerRef, sprite);
            m->setZValue(zValue() - 1);
            m->setPos(x, y);
            scene()->addItem(m);
            meteors.append(m);
        }

        // Cuando se acabe la ventana de 5 s de lluvia, terminamos el ataque
        if (meteorDuration <= 0.0) {
            meteorActive = false;
            finishAttack();
        }
        break;

    case Recover:
        if (stateTimer >= attackCooldown) {
            state = Idle;
            stateTimer = 0.0;
        }
        break;
    }
}


void Boss3::startNextAttack()
{
    stateTimer = 0.0;

    // Versión simple: elige aleatoriamente uno de los 3 ataques
    // 0 = Boomerang, 1 = Jump, 2 = Meteor
    int choice = QRandomGenerator::global()->bounded(3); // devuelve 0, 1 o 2

    switch (choice) {
    case 0:
        startBoomerang();
        break;
    case 1:
        startJumpAttack();
        break;
    case 2:
    default:
        startMeteorRain();
        break;
    }
}


void Boss3::startBoomerang()
{
    qDebug() << "[Boss] Ataque Boomerang";

    if (boomerangActive) {
        // Ya hay uno volando, por seguridad no disparamos otro
        return;
    }

    if (!scene() || !playerRef) {
        // Si no hay escena o jugador, salimos a Recover
        state      = Recover;
        stateTimer = 0.0;
        animState  = AIdle;
        return;
    }

    vx = 0.0;

    QPixmap sprite = boomerangSprite;
    if (sprite.isNull() && !boomCastFrames.isEmpty())
        sprite = boomCastFrames[0];
    else if (sprite.isNull() && !idleFrames.isEmpty())
        sprite = idleFrames[0];

    boomerang = new BoomerangProjectile(this, playerRef, sprite);
    boomerang->setZValue(zValue() + 1);
    boomerang->setPos(pos());

    scene()->addItem(boomerang);

    boomerangActive = true;

    state      = DoBoomerang;
    stateTimer = 0.0;
    currentCastFrames = &boomCastFrames;
    animState  = ACast;

    statsBoomerang.used++;
}


void Boss3::startJumpAttack()
{
    qDebug() << "[Boss] Ataque de salto";

    if (playerRef) {
        qreal dx = playerRef->x() - x();
        vx = (dx > 0) ? 220.0 : -220.0;
        facing = (dx > 0) ? 1 : -1;
    } else {
        vx = 0.0;
    }

    vy = -1000.0;
    state      = DoJump;
    stateTimer = 0.0;
    animState  = AJump;

    statsJump.used++;
}

void Boss3::startMeteorRain()
{
    qDebug() << "[Boss] Lluvia de meteoritos";

    if (scene()) {
        QRectF r = scene()->sceneRect();
        meteorTargetX = (r.left() + r.right()) * 0.5;
        meteorTargetY = 500.0;
    } else {
        meteorTargetX = x();
        meteorTargetY = y();
    }
    meteorActive       = false;
    meteorDuration     = 5.0;
    meteorSpawnTimer   = 0.0;

    vx = 0.0;
    vy = 0.0;

    state      = MoveToMeteorPos;
    stateTimer = 0.0;
    currentCastFrames = &meteorCastFrames;
    animState  = ACast;

    statsMeteor.used++;
}


void Boss3::finishAttack()
{
    qDebug() << "[Boss] Fin de ataque";

    vx = 0.0;
    state      = Recover;
    stateTimer = 0.0;
    animState  = AIdle;
}

/* ===================== ANIMACIÓN ===================== */

void Boss3::updateAnimation(qreal dt)
{
    animTimer += dt;

    int frameCount = 1;
    switch (animState) {
    case AIdle: frameCount = idleFrames.size(); break;
    case ACast:
        if (currentCastFrames && !currentCastFrames->isEmpty())
            frameCount = currentCastFrames->size();
        else
            frameCount = castFrames.size();
        break;
    case AJump:
    case AFall: frameCount = jump.size(); break;
    }

    if (frameCount <= 0) return;

    while (animTimer >= frameDuration) {
        animTimer  -= frameDuration;
        currentFrame = (currentFrame + 1) % frameCount;
    }

    applyAnimFrame();
}

void Boss3::applyAnimFrame()
{
    QPixmap frame;

    switch (animState) {
    case AIdle:
        if (!idleFrames.isEmpty())
            frame = idleFrames[currentFrame % idleFrames.size()];
        break;

    case ACast:
        if (currentCastFrames && !currentCastFrames->isEmpty()) {
            frame = (*currentCastFrames)[currentFrame % currentCastFrames->size()];
        } else if (!castFrames.isEmpty()) {
            frame = castFrames[currentFrame % castFrames.size()];
        } else if (!idleFrames.isEmpty()) {
            frame = idleFrames[0];
        }
        break;

    case AJump:
    case AFall:
        if (!jump.isEmpty())
            frame = jump[currentFrame % jump.size()];
        else if (!idleFrames.isEmpty())
            frame = idleFrames[0];
        break;
    }

    if (frame.isNull())
        return;

    if (facing == -1) {
        QTransform t;
        t.scale(-1, 1);
        frame = frame.transformed(t, Qt::FastTransformation);
    }

    setPixmap(frame);
    setOffset(-pixmap().width() / 2.0, -pixmap().height() / 2.0);
}


void Boss3::setBoomCastSprites(const QVector<QPixmap> &frames)
{
    boomCastFrames = frames;
}

void Boss3::setMeteorCastSprites(const QVector<QPixmap> &frames)
{
    meteorCastFrames = frames;
}

void Boss3::onBoomerangReturned(BoomerangProjectile *b)
{
    if (boomerang == b) {
        boomerangActive = false;

        if (b->scene()) {
            b->scene()->removeItem(b);
        }
        delete b;
        boomerang = nullptr;
    }
}

void Boss3::updateHpBar()
{
    if (!hpFg) return;

    qreal ratio = 0.0;
    if (m_maxHealth > 0)
        ratio = static_cast<qreal>(m_health) / static_cast<qreal>(m_maxHealth);

    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;

    // Ajustamos solo el ancho de la barra verde
    QRectF r = hpFg->rect();
    r.setWidth(hpBarWidth * ratio);
    hpFg->setRect(r);
}
