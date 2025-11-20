#include "level1scene.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QGraphicsRectItem>

Level1Scene::Level1Scene(QObject *parent)
    : GameScene(parent), player(nullptr), tickTimer(new QTimer(this)), lastTimeMs(0)
{
    setSceneRect(0,0,800,600);
    setupScene();

    connect(tickTimer, &QTimer::timeout, this, &Level1Scene::onTick);
    tickTimer->start(16); // ~60 FPS
    lastTimeMs = QDateTime::currentMSecsSinceEpoch();
}

Level1Scene::~Level1Scene()
{
    tickTimer->stop();
    // Qt will delete items added to the scene automatically when scene is deleted
}

void Level1Scene::setupScene()
{
    // Background placeholder
    QGraphicsRectItem *bg = addRect(sceneRect(), QPen(Qt::NoPen), QBrush(Qt::darkGray));
    bg->setZValue(-10);

    // Add a simple title/hint
    addText("Nivel 1: Redención en la Tierra")->setPos(10,10);

    // Create player and add to scene center
    player = new Player();
    addItem(player);
    player->setPos(sceneRect().width()/2, sceneRect().height()/2);
    entities.push_back(player);

    // Example static obstacle (placeholder)
    QGraphicsRectItem *obs = addRect(200, 200, 120, 40, QPen(), QBrush(Qt::gray));
    obs->setZValue(1);

    // Later: spawn enemies, crucifixes, etc.
}

void Level1Scene::onEnter() { /* nothing now */ }
void Level1Scene::onExit() { /* stop timers or sounds if any */ }

void Level1Scene::onTick()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qreal dt = (now - lastTimeMs) / 1000.0; // seconds
    lastTimeMs = now;

    updateEntities(dt);
    // refresh view bounding
    update();
}

void Level1Scene::updateEntities(qreal dt)
{
    for (Entity* e : entities) {
        if (e) e->updateEntity(dt);

        if (e == player) {

            // obtener posición actual
            QPointF pos = player->pos();

            // obtener límites del mapa
            QRectF bounds = sceneRect();

            // tamaño del sprite del jugador
            qreal halfW = player->pixmap().width() / 2.0;
            qreal halfH = player->pixmap().height() / 2.0;

            // ❗ asegurarse de que el jugador NO salga del mapa
            if (pos.x() < bounds.left() + halfW)
                pos.setX(bounds.left() + halfW);

            if (pos.x() > bounds.right() - halfW)
                pos.setX(bounds.right() - halfW);

            if (pos.y() < bounds.top() + halfH)
                pos.setY(bounds.top() + halfH);

            if (pos.y() > bounds.bottom() - halfH)
                pos.setY(bounds.bottom() - halfH);

            // aplicar posición corregida
            player->setPos(pos);
        }
    }
    // collisions and game logic will be added here
}

void Level1Scene::keyPressEvent(QKeyEvent *event)
{
    if (!player) return;
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        player->moveLeft(true);
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        player->moveRight(true);
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        player->moveUp(true);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        player->moveDown(true);
        break;
    default:
        QGraphicsScene::keyPressEvent(event);
    }
}

void Level1Scene::keyReleaseEvent(QKeyEvent *event)
{
    if (!player) return;
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        player->moveLeft(false);
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        player->moveRight(false);
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        player->moveUp(false);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        player->moveDown(false);
        break;
    default:
        QGraphicsScene::keyReleaseEvent(event);
    }
}
