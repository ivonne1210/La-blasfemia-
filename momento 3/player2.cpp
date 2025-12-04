#include "player2.h"
#include <QKeyEvent>
#include <QPainter>

using namespace std;

int lastDirection1;

Player2::Player2(QGraphicsItem *parent)
    : Actor(parent),
    left(false), right(false), up(false), down(false),
    m_speed(180),
    m_health(100),
    currentFrame(0),
    invincibleTime(0)
{
    for (int i = 1; i <= 7; i++) {
        QString path = QString("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/player2_%1.png").arg(i);
        QPixmap frame(path);

        frame = frame.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        walkFrames.push_back(frame);
    }

    lastDirection1 = 0;  // empieza mirando abajo

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

void Player2::updateEntity(qreal dt)
{
    QPointF vel(0,0);
    if (left) vel.setX(-m_speed);
    if (right) vel.setX(m_speed);
    if (up) vel.setY(0);
    if (down) vel.setY(0);

    setVelocity(vel);
    Actor::updateEntity(dt);
    if (invincibleTime > 0)
    {
        invincibleTime -= dt;
        blinkTimer += dt;

        if (blinkTimer > 0.1f) {
            blinkTimer = 0;
            setVisible(!isVisible());
        }

        if (invincibleTime <= 0) {
            setVisible(true);
            blinking = false;
        }
    }

    storeLastSafePos();
}

void Player2::moveLeft(bool on) { left = on; }
void Player2::moveRight(bool on) { right = on; }
void Player2::moveUp(bool on) { up = on; }
void Player2::moveDown(bool on) { down = on; }
void Player2::defense(bool on) { defen = on; }
void Player2::atack(bool on) { atc = on; }


void Player2::updateAnimation()
{
    int startFrame = 0;
    int endFrame = 0;
    int idleFrame = 0;
    bool moving = (left || right || up || down || defen || atc);

    if (down && !up) {
        startFrame = 0; endFrame = 0; idleFrame = 0;
        lastDirection1 = 0;
    }
    else if (up && !down) {
        startFrame = 0; endFrame = 2; idleFrame = 0;
        lastDirection1 = 0;
    }
    else if (right && !left) {
        startFrame = 3; endFrame = 3; idleFrame = 0;
        lastDirection1 = 1;
    }
    else if (left && !right) {
        startFrame = 4; endFrame = 4; idleFrame = 0;
        lastDirection1 = 1;
    }
    else if(defen && !up && !right && !left && !down && !atc){
        startFrame = 5; endFrame = 5; idleFrame = 0;
        lastDirection1 = 0;
    }
    else if(atc && !up && !right && !left && !down && !defen){
        startFrame = 6; endFrame = 6; idleFrame = 0;
        lastDirection1 = 0;
    }
    else {
        moving = false;
    }

    if (!moving)
    {
        switch (lastDirection1)
        {
        case 0: idleFrame = 0; break;
        case 1: idleFrame = 0; break;
        case 2: idleFrame = 5; break;
        case 3: idleFrame = 10; break;
        }

        currentFrame = idleFrame;
        setPixmap(walkFrames[currentFrame]);
        setOffset(-pixmap().width()/2, -pixmap().height()/2);
        return;
    }

    if (currentFrame < startFrame || currentFrame > endFrame)
        currentFrame = startFrame;
    else
        currentFrame++;

    if (currentFrame > endFrame)
        currentFrame = startFrame;

    setPixmap(walkFrames[currentFrame]);
    setOffset(-pixmap().width()/2, -pixmap().height()/2);
}

void Player2::takeDamage(int dmg)
{
    if (invincibleTime > 0) return;  // aún invencible

    m_health -= dmg;
    if (m_health < 0) m_health = 0;

    invincibleTime = 0.6f; // 600ms sin recibir más daño
    blinking = true;
    blinkTimer = 0;
    qDebug() << "Jugador recibe daño =" << dmg << " Salud =" << m_health;
}

void Player2::storeLastSafePos()
{
    lastSafePos = pos();
}

void Player2::restoreLastSafePos()
{
    setPos(lastSafePos);
}



