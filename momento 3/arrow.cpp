#include "arrow.h"
#include <QGraphicsScene>

Arrow::Arrow(QPointF startPos, int dir, float speed, int dmg)
    : direction(dir), velocity(speed), damage(dmg)
{
    // ---- CARGAR SPRITES ----
    spriteUp = QPixmap("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/arrow1.png").scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spriteDown = QPixmap("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/arrow2.png").scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // ---- Elegir sprite basado en dirección ----
    if (direction == -1)
        setPixmap(spriteUp);
    else
        setPixmap(spriteDown);

    // ---- Posición inicial ----
    setPos(startPos.x(), startPos.y() - 50);
    setZValue(500);
}

void Arrow::updateEntity(qreal dt)
{
    // Movimiento vertical
    setY(y() + direction * velocity * dt);

    if (!scene()) return;

    if (y() < 0 || y() > scene()->sceneRect().bottom()) {
        scene()->removeItem(this);
        delete this;
    }
}
