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
    //idleFrames.append(QPixmap(base + "idle_1.png"));

    // Run
    //runFrames.append(QPixmap(base + "run_0.png"));
    //runFrames.append(QPixmap(base + "run_1.png"));
    //runFrames.append(QPixmap(base + "run_2.png"));
    //runFrames.append(QPixmap(base + "run_3.png"));

    // Jump
    //jumpFrames.append(QPixmap(base + "jump_0.png"));

    // Fall
    {
        QPixmap p(base + "player3_2.png");
        if (!p.isNull()) {
            p = p.scaled(SPRITE_W, SPRITE_H,
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
            fallFrames.append(p);
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
    // Solo nos interesa el flanco de subida (false -> true)
    if (pressed && !jumpPressed) {
        // Permitir salto si prácticamente no se mueve en Y
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
    // Movimiento horizontal
    if (leftPressed && !rightPressed) {
        vx = -moveSpeed;
    } else if (rightPressed && !leftPressed) {
        vx = moveSpeed;
    } else {
        vx = 0.0;
    }

    // Gravedad
    vy += gravity * dt;

    // Integración de posición
    QPointF pos = this->pos();
    pos.setX(pos.x() + vx * dt);
    pos.setY(pos.y() + vy * dt);

    // Piso plano por ahora (groundY)
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
    // "Grounded" = piso global o plataforma
    bool grounded = (onGround || inPlat);

    AnimState newState = animState;

    if (!grounded) {
        // Está en el aire
        if (vy < 0)
            newState = JumpState;
        else
            newState = FallState;
    } else {
        // Está sobre algo (piso o plataforma)
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
    case Idle:      frameCount = idleFrames.size(); break;
    case Run:       frameCount = runFrames.size();  break;
    case JumpState: frameCount = jumpFrames.size(); break;
    case FallState: frameCount = fallFrames.size(); break;
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
    }

    if (frame.isNull()) return;

    // Voltear si mira a la izquierda
    if (facing == -1) {
        QTransform t;
        t.scale(-1, 1);
        frame = frame.transformed(t, Qt::FastTransformation);
    }

    setPixmap(frame);
}

void Player3::setVerticalVelocity(qreal v)
{
    vy = v;
}

