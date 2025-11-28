#include "enemy2.h"
#include <QDebug>
#include <QGraphicsScene>

Enemy2::Enemy2(QPointF pos, int health)
    : Enemy(pos, pos, 0, 0), // velocidad y destino no importan
    health_(health)
{
    // Cargar sprite diferente
    pix = QPixmap("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/enemy2_1.png").scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(pix);

    setPos(pos);
}

void Enemy2::takeDamage(int dmg)
{
    health_ -= dmg;
}


void Enemy2::updateEntity(qreal dt)
{
    if (!canMove) return;

    float newX = x() + speed * dt;

    float minX = 360;
    float maxX = 600;

    // Si toca borde → invertir dirección
    if (newX < minX) {
        newX = minX;
        speed = -speed; // gira a la derecha
    }
    else if (newX > maxX) {
        newX = maxX;
        speed = -speed; // gira a la izquierda
    }

    setX(newX);
}
