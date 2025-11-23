#include "level1scene.h"
#include "enemy.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QGraphicsRectItem>
#include <QRandomGenerator>


Level1Scene::Level1Scene(QObject *parent)
    : GameScene(parent), player(nullptr), tickTimer(new QTimer(this)), lastTimeMs(0)
{
    QPixmap map("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/back1.png");
    setSceneRect(0, 0, map.width(), map.height());
    setupScene();

    connect(tickTimer, &QTimer::timeout, this, &Level1Scene::onTick);
    tickTimer->start(16);
    lastTimeMs = QDateTime::currentMSecsSinceEpoch();
}

Level1Scene::~Level1Scene()
{
    tickTimer->stop();
}

void Level1Scene::setupScene()
{
    QPixmap map("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/back1.png");
    QGraphicsPixmapItem* mapItem = addPixmap(map);
    mapItem->setPos(0, 0);
    mapItem->setZValue(-100);

    player = new Player();
    addItem(player);
    player->setPos(sceneRect().width()/2, sceneRect().height()/2);
    entities.push_back(player);

    Item* item1 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/item1.png");
    item1->setPos(50, 500);
    addItem(item1);
    Item* item2 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/item2.png");
    item2->setPos(100, 500);
    addItem(item2);
    Item* item3 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/item3.png");
    item3->setPos(150, 500);
    addItem(item3);
    Item* item4 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/item4.png");
    item4->setPos(200, 500);
    addItem(item4);
    Item* item5 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/item5.png");
    item5->setPos(250, 500);
    addItem(item5);

    items.push_back(item1);
    items.push_back(item2);
    items.push_back(item3);
    items.push_back(item4);
    items.push_back(item5);

    QPainterPath backPath;
    backPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBack = addPath(backPath, QPen(Qt::black), QBrush(Qt::darkGray));
    healthBack->setZValue(1000);

    QPainterPath barPath;
    barPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBar = addPath(barPath, QPen(Qt::NoPen), QBrush(QColor(132,41,30)));
    healthBar->setZValue(1001);
    spawnRandomEnemies(3);
}

void Level1Scene::onEnter() { /* nothing now */ }
void Level1Scene::onExit() { /* stop timers or sounds if any */ }

void Level1Scene::onTick()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qreal dt = (now - lastTimeMs) / 1000.0; // seconds
    lastTimeMs = now;

    if (viewRef && player) {
        viewRef->centerOn(player);
    }

    updateEntities(dt);
    updateHealthBar();
    updateHud();
    update();
}

void Level1Scene::updateEntities(qreal dt)
{
    for (Entity* e : entities) {
        if (e) e->updateEntity(dt);

        if (e == player) {

            QPointF pos = player->pos();

            QRectF bounds(0, 385, sceneRect().width(), sceneRect().height() - 385);

            qreal halfW = player->pixmap().width() / 2.0;
            qreal halfH = player->pixmap().height() / 2.0;

            if (pos.x() < bounds.left() + halfW)
                pos.setX(bounds.left() + halfW);

            if (pos.x() > bounds.right() - halfW)
                pos.setX(bounds.right() - halfW);

            if (pos.y() < bounds.top() + halfH)
                pos.setY(bounds.top() + halfH);

            if (pos.y() > bounds.bottom() - halfH)
                pos.setY(bounds.bottom() - halfH);

            player->setPos(pos);
        }
    }
    for (Item* it : items)
    {
        if (player->collidesWithItem(it))
        {
            qDebug() << "Jugador recogió un ítem!";

            removeItem(it);
            items.erase(std::remove(items.begin(), items.end(), it), items.end());
            delete it;
            break;
        }
    }

    // --- ENEMIGOS: DETECTOR DE DAÑO Y RETROCESO ---
    for (Entity *ent : entities)
    {
        Enemy *enemy = dynamic_cast<Enemy*>(ent);
        if (!enemy) continue;

        if (player->collidesWithItem(enemy))
        {
            player->takeDamage(enemy->getDamage());

            // Retrocede al jugador
            player->restoreLastSafePos();
        }
    }

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

void Level1Scene::setView(QGraphicsView *v)
{
    viewRef = v;
}

void Level1Scene::updateHealthBar()
{
    if (!player || !healthBar) return;

    float maxWidth = 200.0f;
    float ratio = player->health() / 100.0f;
    if (ratio < 0) ratio = 0;

    QPainterPath barPath;
    barPath.addRoundedRect(0, 0, maxWidth * ratio, 20, 8, 8);
    healthBar->setPath(barPath);}

void Level1Scene::updateHud()
{
    if (!viewRef || !healthBack || !healthBar) return;

    QPointF topRight = viewRef->mapToScene(viewRef->viewport()->rect().topRight());

    float hudX = topRight.x() - 220;  // margen derecha
    float hudY = topRight.y() + 20;   // margen arriba

    healthBack->setPos(hudX, hudY);
    healthBar->setPos(hudX, hudY);
}

void Level1Scene::spawnRandomEnemies(int count)
{
    QRectF playArea(0, 385, sceneRect().width(), sceneRect().height() - 385);
    auto *rng = QRandomGenerator::global();

    for (int i = 0; i < count; i++)
    {
        // Punto inicial aleatorio
        qreal x1 = playArea.left() + rng->bounded(int(playArea.width()));
        qreal y1 = playArea.top()  + rng->bounded(int(playArea.height()));

        // Punto final aleatorio
        qreal x2 = playArea.left() + rng->bounded(int(playArea.width()));
        qreal y2 = playArea.top()  + rng->bounded(int(playArea.height()));

        Enemy *e = new Enemy(QPointF(x1, y1), QPointF(x2, y2), 60, 10);
        addItem(e);
        entities.push_back(e);
    }
}

