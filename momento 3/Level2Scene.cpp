#include "level2scene.h"
#include "enemy2.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QGraphicsRectItem>
#include <QRandomGenerator>

Level2Scene::Level2Scene(QObject *parent)
    : GameScene(parent), player(nullptr), tickTimer(new QTimer(this)), lastTimeMs(0)
{
    QPixmap map("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/back1.png");
    setSceneRect(0, 0, map.width(), map.height());
    setupScene();

    connect(tickTimer, &QTimer::timeout, this, &Level2Scene::onTick);
    tickTimer->start(16);
    lastTimeMs = QDateTime::currentMSecsSinceEpoch();

    QPainterPath backPath;
    backPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBack = addPath(backPath, QPen(Qt::black), QBrush(Qt::darkGray));
    healthBack->setZValue(1000);

    QPainterPath barPath;
    barPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBar = addPath(barPath, QPen(Qt::NoPen), QBrush(QColor(132,41,30)));
    healthBar->setZValue(1001);
}

Level2Scene::~Level2Scene()
{
    tickTimer->stop();
}

void Level2Scene::setupScene()
{
    // === CARGAR EL FONDO ===
    QPixmap map("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/back2.png");

    if (map.isNull()) {
        setBackgroundBrush(Qt::black);
    }

    // Ajustar tamaño de la escena AL TAMAÑO DEL PNG
    setSceneRect(0, 0, map.width(), map.height());

    // === CREAR EL ÚNICO ITEM DE FONDO ===
    background = new QGraphicsPixmapItem(map);
    background->setZValue(-100);
    background->setPos(0, 0);
    addItem(background);

    player = new Player2();
    addItem(player);
    player->setPos(525, 800);
    entities.push_back(player);
    lastPlayerY = 0;

    // --- Música de fondo ---
    musicPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(audioOutput);

    musicPlayer->setSource(QUrl::fromLocalFile(
        "C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/Lvl1S.mp3"
        ));

    audioOutput->setVolume(1);
    musicPlayer->setLoops(QMediaPlayer::Infinite);
    musicPlayer->play();
}

void Level2Scene::onEnter()
{
    if (viewRef && player)
        viewRef->centerOn(player);
}
void Level2Scene::onExit() {}

void Level2Scene::onTick()
{
    if (gameOver) return;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qreal dt = (now - lastTimeMs) / 1000.0;
    lastTimeMs = now;

    updateEntities(dt);
    updateHealthBar();
    updateHud();
    update();
    enemy2Shoot();
}

void Level2Scene::updateEntities(qreal dt)
{
    for (Entity* e : entities) {
        if (e) e->updateEntity(dt);

        if (e == player) {

            QPointF pos = player->pos();

            float minX = 200;
            float maxX = 880;
            float minY = 500;
            float maxY = 856;

            float halfW = player->pixmap().width() / 2.0f;
            float halfH = player->pixmap().height() / 2.0f;

            if (pos.x() < minX + halfW)
                pos.setX(minX + halfW);

            if (pos.x() > maxX - halfW)
                pos.setX(maxX - halfW);

            if (pos.y() < minY + halfH)
                pos.setY(minY + halfH);

            if (pos.y() > maxY - halfH)
                pos.setY(maxY - halfH);

            player->setPos(pos);
            if (player->up){lastPlayerY+=1;}
        }
    }
    bool hayEnemigos = false;

    for (Entity* ent : entities)
    {
        Enemy2* enemy = dynamic_cast<Enemy2*>(ent);
        if (enemy) {
            hayEnemigos = true;
            break;
        }
    }

    if (!hayEnemigos)
    {
        travelledDistance = lastPlayerY;

        if (travelledDistance >= nextHordeAt)
        {
            spawnHorde();
        }
    }

    for (Entity* e : entities)
    {
        if (!e) continue;

        Arrow* arrow = dynamic_cast<Arrow*>(e);
        if (arrow)
        {
            // eliminar la flecha si sale del mapa
            if (arrow->direction == -1 && arrow->y() < 580)
            {
                removeItem(arrow);
                e = nullptr;
                continue;
            }
            else if(arrow->direction == 1 && arrow->y() > 820)
            {
                removeItem(arrow);
                e = nullptr;
                continue;
            }

            // Revisar colisión con enemigos
            for (Entity* other : entities)
            {
                Enemy2* enemy = dynamic_cast<Enemy2*>(other);
                if (!enemy) continue;
                if (arrow->collidesWithItem(enemy) && arrow->direction == -1)
                {
                    qDebug() << "Flecha golpeó enemigo";
                    enemy->takeDamage(arrow->getDamage());
                    removeItem(arrow);
                    entities.erase(std::remove(entities.begin(), entities.end(), arrow), entities.end());
                    delete arrow;
                    if(enemy->getHealth()<=0){
                        removeItem(enemy);
                        entities.erase(std::remove(entities.begin(), entities.end(), enemy), entities.end());
                        delete enemy;
                    }
                    break;
                }
                if (arrow->collidesWithItem(player) && arrow->direction == 1)
                {
                    if(player->defen){
                    }
                    else{
                    player->takeDamage(arrow->getDamage());
                    }
                    removeItem(arrow);
                    entities.erase(std::remove(entities.begin(), entities.end(), arrow), entities.end());
                    delete arrow;
                    break;
                }
            }
        }
    }
}

void Level2Scene::keyPressEvent(QKeyEvent *event)
{
    if (!player) return;
    if (event->isAutoRepeat()) return;
    if (gameOver) return;

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

        if (travelledDistance<=nextHordeAt)
            lastPlayerY += 4;
        player->moveUp(true);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        player->moveDown(false);
        break;
    case Qt::Key_K:
        player->defense(true);
        break;
    case Qt::Key_L:{
        // dirección del jugador
        int dir = -1;

        // posición de inicio EXACTA del jugador
        QPointF start = player->pos();
        int dmg = 0;
        if(numeroH<3){
            dmg = 25;
        }
        else{
            dmg = 10;
        }
        Arrow* arrow = new Arrow(start,dir,150,dmg);

        addItem(arrow);
        entities.push_back(arrow);
        player->atack(true);
        break;}
    default:
        QGraphicsScene::keyPressEvent(event);
    }
}

void Level2Scene::keyReleaseEvent(QKeyEvent *event)
{
    if (!player) return;
    if (event->isAutoRepeat()) return;
    if (gameOver) return;

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
    case Qt::Key_K:
        player->defense(false);
        break;
    case Qt::Key_L:
        player->atack(false);
        break;
    default:
        QGraphicsScene::keyReleaseEvent(event);
    }
}

void Level2Scene::setView(QGraphicsView *v)
{
    viewRef = v;
}

void Level2Scene::updateHealthBar()
{
    if (!player || !healthBar) return;

    float maxWidth = 200.0f;
    float ratio = player->health() / 100.0f;
    if (ratio < 0) ratio = 0;

    QPainterPath barPath;
    barPath.addRoundedRect(0, 0, maxWidth * ratio, 20, 8, 8);
    healthBar->setPath(barPath);}

void Level2Scene::updateHud()
{
    if (!viewRef || !healthBack || !healthBar) return;

    QPointF topRight = viewRef->mapToScene(viewRef->viewport()->rect().topRight());

    float hudX = topRight.x() - 220;  // margen derecha
    float hudY = topRight.y() + 20;   // margen arriba

    healthBack->setPos(hudX, hudY);
    healthBar->setPos(hudX, hudY);
}

void Level2Scene::spawnRandomEnemies(int starX, int starY)
{
    Enemy2 *e = new Enemy2(QPointF(starX, starY), 50);
    addItem(e);
    entities.push_back(e);
}

void Level2Scene::triggerGameOver()
{
    gameOver = true;

    tickTimer->stop();  // Detener actualizaciones
    if (musicPlayer) musicPlayer->stop();

    // Crear texto "GAME OVER"
    gameOverText = new QGraphicsTextItem("GAME OVER");
    gameOverText->setDefaultTextColor(QColor(132,41,30));
    gameOverText->setFont(QFont("Old English Text MT", 64, QFont::Bold));

    // Centrar el texto respecto a la cámara
    QPointF center = viewRef->mapToScene(
        viewRef->viewport()->rect().center()
        );

    gameOverText->setPos(center.x() - 300, center.y() - 100);
    gameOverText->setZValue(9999);

    addItem(gameOverText);
}

void Level2Scene::goToNextLevel()
{
    if (musicPlayer) musicPlayer->stop();
    tickTimer->stop();
    emit levelCompleted();
}

void Level2Scene::spawnHorde()
{
    travelledDistance = 0;
    lastPlayerY = 0;
    numeroH +=1;

    switch(numeroH){
    case 1:
        spawnRandomEnemies(540, 510);
        spawnRandomEnemies(420, 510);
        break;
    case 2:
        spawnRandomEnemies(600, 570);
        spawnRandomEnemies(360, 570);
        break;
    case 3:
        spawnRandomEnemies(540, 510);
        spawnRandomEnemies(420, 510);
        spawnRandomEnemies(600, 570);
        spawnRandomEnemies(360, 570);
        break;
    case 4:
        spawnRandomEnemies(600, 570);
        spawnRandomEnemies(360, 570);
        spawnRandomEnemies(480, 570);
        break;
    case 5:
        spawnRandomEnemies(600, 570);
        spawnRandomEnemies(360, 570);
        break;
    case 6:
        spawnRandomEnemies(600, 570);
        spawnRandomEnemies(360, 570);
        spawnRandomEnemies(480, 510);
        break;
    case 7:
        spawnRandomEnemies(540, 510);
        spawnRandomEnemies(420, 510);
        spawnRandomEnemies(600, 570);
        spawnRandomEnemies(360, 570);
        break;
    default:
        break;
    }

    if (numeroH >= 5)
    {
        for (Entity* e : entities)
        {
            Enemy2* enemy = dynamic_cast<Enemy2*>(e);
            if (enemy)
                enemy->enableMovement(true);
        }
    }

}

void Level2Scene::enemy2Shoot()
{
    if (!player) return;

    for (Entity* e : entities)
    {
        Enemy2* enemy = dynamic_cast<Enemy2*>(e);
        if (!enemy) continue;

        int chance = QRandomGenerator::global()->bounded(1000);
        if (chance < 10)
        {
            QPointF start = enemy->pos();
            int direction = 1;

            Arrow* arrow = new Arrow(QPointF(start.x(),start.y()+100), direction, 150, 10);
            addItem(arrow);
            entities.push_back(arrow);
        }
    }
}


