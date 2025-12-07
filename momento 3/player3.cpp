#include "player3.h"
#include <QTransform>
#include <QtMath>
#include <QDebug>

Player3::Player3(QGraphicsItem *parent)
    : Actor(parent),
    m_health(100)
{
    // ==== CARGAR SPRITES ====
    // Ajusta la ruta base a tu carpeta de sprites:
    const QString base = "C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/";

    const int SPRITE_W = 90;
    const int SPRITE_H = 90;
    // Idle (ej: idle_0.png, idle_1.png, ...)
    // Idle
    {
        QPixmap p(base + "player3_1.png");
        if (!p.isNull()) {
            p = p.scaled(SPRITE_W, SPRITE_H,
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
            idleFrames.append(p);
        }
    }
    {
        QPixmap p(base + "player3_2.png");
        if (!p.isNull()) {
            p = p.scaled(SPRITE_W, SPRITE_H,
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
            fallFrames.append(p);
        }
    }

    {
        QPixmap c(base + "player3_3.png");
        if (!c.isNull()) {
            c = c.scaled(SPRITE_W, SPRITE_H,
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
            QTransform t; t.scale(-1, 1); c = c.transformed(t, Qt::SmoothTransformation);
            guardFrames.append(c);
        }
    }

    {
        QPixmap p(base + "player3_4.png");
        if (!p.isNull()) {
            p = p.scaled(SPRITE_W+50, SPRITE_H+50,
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
            QTransform t; t.scale(-1, 1); p = p.transformed(t, Qt::SmoothTransformation);
            atackFrames.append(p);
        }
    }
    // Frame inicial
    if (!idleFrames.isEmpty()) {
        setPixmap(idleFrames[0]);
        setOffset(-pixmap().width()/2, -pixmap().height()/2);
    }
    setZValue(10);
}

void Player3::moveLeft(bool pressed)
{
    leftPressed = pressed;
    if (pressed) facing = -1;
}

void Player3::moveRight(bool pressed)
{
    rightPressed = pressed;
    if (pressed) facing = 1;
}

void Player3::jump(bool pressed)
{
    if (pressed && !jumpPressed) {
        if (qFabs(vy) < 1.0) {   // está "de pie" (piso o plataforma)
            vy = jumpSpeed;     // impulso hacia arriba
            // onGround = false; // opcional, si lo sigues usando
        }
    }
    jumpPressed = pressed;
}

void Player3::updateEntity(qreal dt)
{
    updatePhysics(dt);
    updateAnimation(dt);
}

void Player3::updatePhysics(qreal dt)
{
    // Si está atacando o defendiendo, lo dejamos quieto en X
    if (guardPressed || attackPressed) {
        vx = 0.0;
    } else {
        if (leftPressed && !rightPressed) {
            vx = -moveSpeed;
        } else if (rightPressed && !leftPressed) {
            vx = moveSpeed;
        } else {
            vx = 0.0;
        }
    }

    // Gravedad
    vy += gravity * dt;

    // Integración de posición
    QPointF pos = this->pos();
    pos.setX(pos.x() + vx * dt);
    pos.setY(pos.y() + vy * dt);

    if (pos.y() >= groundY) {
        pos.setY(groundY);
        vy = 0.0;
        onGround = true;
    } else {
        onGround = false;
    }

    setPos(pos);
}


void Player3::updateAnimation(qreal dt)
{
    bool grounded = (onGround || inPlat);

    AnimState newState = animState;

    // 🔺 Prioridad: Attack > Guard > aire > caminar/idle
    if (attackPressed) {
        newState = Attack;
    }
    else if (guardPressed) {
        newState = Guard;
    }
    else if (!grounded) {
        if (vy < 0)
            newState = JumpState;
        else
            newState = FallState;
    } else {
        if (qFabs(vx) > 5.0)
            newState = Run;
        else
            newState = Idle;
    }

    if (newState != animState) {
        animState = newState;
        currentFrame = 0;
        animTimer = 0.0;
    }

    animTimer += dt;

    int frameCount = 0;
    switch (animState) {
    case Idle:       frameCount = idleFrames.size();  break;
    case Run:        frameCount = runFrames.size();   break;
    case JumpState:  frameCount = jumpFrames.size();  break;
    case FallState:  frameCount = fallFrames.size();  break;
    case Guard:      frameCount = guardFrames.size(); break;
    case Attack:     frameCount = atackFrames.size(); break;
    }

    if (frameCount <= 0) return;

    while (animTimer >= frameDuration) {
        animTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % frameCount;
    }

    applyAnimFrame();
}



void Player3::applyAnimFrame()
{
    QPixmap frame;

    switch (animState) {
    case Idle:
        if (!idleFrames.isEmpty())
            frame = idleFrames[currentFrame % idleFrames.size()];
        break;

    case Run:
        if (!runFrames.isEmpty())
            frame = runFrames[currentFrame % runFrames.size()];
        break;

    case JumpState:
        if (!jumpFrames.isEmpty())
            frame = jumpFrames[currentFrame % jumpFrames.size()];
        break;

    case FallState:
        if (!fallFrames.isEmpty())
            frame = fallFrames[currentFrame % fallFrames.size()];
        break;

    case Guard:
        if (!guardFrames.isEmpty())
            frame = guardFrames[currentFrame % guardFrames.size()];
        break;

    case Attack:
        if (!atackFrames.isEmpty())
            frame = atackFrames[currentFrame % atackFrames.size()];
        break;
    }

    if (frame.isNull()) return;

    if (facing == -1) {
        QTransform t;
        t.scale(-1, 1);
        frame = frame.transformed(t, Qt::FastTransformation);
    }

    setPixmap(frame);
    setOffset(-pixmap().width()/2, -pixmap().height()/2);
}


void Player3::setVerticalVelocity(qreal v)
{
    vy = v;
}

void Player3::setGuard(bool pressed)
{
    guardPressed = pressed;
}

void Player3::setAttack(bool pressed)
{
    attackPressed = pressed;
}


