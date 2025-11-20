#include "player.h"
#include <QKeyEvent>
#include <QPainter>

using namespace std;

int lastDirection;

Player::Player(QGraphicsItem *parent)
    : Actor(parent),
    left(false), right(false), up(false), down(false),
    m_speed(180),
    currentFrame(0)
{
    // Cargar todos los frames localmente (temporal)
    for (int i = 0; i < 12; i++) {
        QString path = QString("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/player%1.png").arg(i);
        QPixmap frame(path);

        frame = frame.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        walkFrames.push_back(frame);
    }

    lastDirection = 0;  // empieza mirando abajo

    // Sprite inicial
    setPixmap(walkFrames[0]);
    setOffset(-pixmap().width()/2, -pixmap().height()/2);

    // Timer de animación
    animTimer = new QTimer();

    QObject::connect(animTimer, &QTimer::timeout, [this]() {
        this->updateAnimation();
    });

    animTimer->start(100);   // 100 ms entre frames = 10 FPS
}

void Player::updateEntity(qreal dt)
{
    QPointF vel(0,0);
    if (left) vel.setX(-m_speed);
    if (right) vel.setX(m_speed);
    if (up) vel.setY(-m_speed);
    if (down) vel.setY(m_speed);

    setVelocity(vel);
    Actor::updateEntity(dt);
}

void Player::moveLeft(bool on) { left = on; }
void Player::moveRight(bool on) { right = on; }
void Player::moveUp(bool on) { up = on; }
void Player::moveDown(bool on) { down = on; }

void Player::updateAnimation()
{
    int startFrame = 0;
    int endFrame = 0;
    int idleFrame = 0;
    bool moving = (left || right || up || down);

    // ---- DETERMINAR DIRECCIÓN ----
    if (down && !up) {
        startFrame = 0; endFrame = 2; idleFrame = 0;
        lastDirection = 0; // abajo
    }
    else if (up && !down) {
        startFrame = 3; endFrame = 6; idleFrame = 6;
        lastDirection = 1; // arriba
    }
    else if (right && !left) {
        startFrame = 7; endFrame = 9; idleFrame = 7;
        lastDirection = 2; // derecha
    }
    else if (left && !right) {
        startFrame = 10; endFrame = 11; idleFrame = 10;
        lastDirection = 3; // izquierda
    }
    else {
        moving = false;
    }

    // ---- IDLE SIN MOVIMIENTO (SE QUEDA EN LA ÚLTIMA DIRECCIÓN) ----
    if (!moving)
    {
        switch (lastDirection)
        {
        case 0: idleFrame = 0; break;   // abajo
        case 1: idleFrame = 3; break;   // arriba
        case 2: idleFrame = 7; break;   // derecha
        case 3: idleFrame = 10; break;  // izquierda
        }

        currentFrame = idleFrame;
        setPixmap(walkFrames[currentFrame]);
        setOffset(-pixmap().width()/2, -pixmap().height()/2);
        return;
    }

    // ---- ANIMACIÓN EN MOVIMIENTO ----
    if (currentFrame < startFrame || currentFrame > endFrame)
        currentFrame = startFrame;
    else
        currentFrame++;

    if (currentFrame > endFrame)
        currentFrame = startFrame;

    setPixmap(walkFrames[currentFrame]);
    setOffset(-pixmap().width()/2, -pixmap().height()/2);
}


