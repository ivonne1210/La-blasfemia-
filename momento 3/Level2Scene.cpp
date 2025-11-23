#include "level2scene.h"
#include <QGraphicsRectItem>
#include <QFont>

Level2Scene::Level2Scene(QObject *parent)
    : GameScene(parent)
{
    // Fondo negro
    setSceneRect(0, 0, 900, 700);
    auto *bg = addRect(sceneRect(), QPen(Qt::NoPen), QBrush(Qt::black));
    bg->setZValue(-10);

    // Texto de nivel 2
    label = addText("NIVEL 2", QFont("Arial", 40, QFont::Bold));
    label->setDefaultTextColor(Qt::white);

    // Centrar texto
    label->setPos(
        sceneRect().width()/2 - label->boundingRect().width()/2,
        sceneRect().height()/2 - label->boundingRect().height()/2
        );
}

Level2Scene::~Level2Scene()
{
}
