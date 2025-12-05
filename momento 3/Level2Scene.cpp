#include "level2scene.h"
#include "enemy2.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QGraphicsRectItem>
#include <QtMath>
#include <QRandomGenerator>

Level2Scene::Level2Scene(QObject *parent)
    : GameScene(parent),
    player(nullptr),
    tickTimer(new QTimer(this)),
    lastTimeMs(0),
    lastSecondTick(0),
    levelTime(80),
    gameOver(false),          // <--- IMPORTANTE
    viewRef(nullptr),
    healthBack(nullptr),
    healthBar(nullptr),
    timerText(nullptr),
    gameOverText(nullptr),
    background(nullptr),
    travelledDistance(0.0f),
    nextHordeAt(100.0f),      // pon el valor que tengas pensado
    lastPlayerY(0.0f),
    numeroH(0),
    musicPlayer(nullptr),
    audioOutput(nullptr)
{
    setupScene();

    QPainterPath backPath;
    backPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBack = addPath(backPath, QPen(Qt::black), QBrush(Qt::darkGray));
    healthBack->setZValue(1000);

    QPainterPath barPath;
    barPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBar = addPath(barPath, QPen(Qt::NoPen), QBrush(QColor(132,41,30)));
    healthBar->setZValue(1001);

    // === TIMER DEL NIVEL ===
    timerText = new QGraphicsTextItem("60");
    timerText->setDefaultTextColor(QColor(132,41,30));
    timerText->setFont(QFont("Old English Text MT", 32, QFont::Bold));
    timerText->setZValue(2000);
    addItem(timerText);

    // posición inicial arriba a la izquierda
    timerText->setPos(20, 20);
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

    setSceneRect(0, 0, map.width(), map.height());

    background = new QGraphicsPixmapItem(map);
    background->setZValue(-100);
    background->setPos(0, 0);
    addItem(background);

    player = new Player2();
    addItem(player);
    player->setPos(525, 800);
    entities.push_back(player);
    lastPlayerY = 0;

    cloudSprite1.load("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/nube1.png");
    cloudSprite2.load("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/nube2.png");

    if (!cloudSprite1.isNull()) {
        QSize newSize(cloudSprite1.width() * 0.7, cloudSprite1.height() * 0.4);
        cloudSprite1 = cloudSprite1.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!cloudSprite2.isNull()) {
        QSize newSize(cloudSprite2.width() * 0.6, cloudSprite2.height() * 0.4);
        cloudSprite2 = cloudSprite2.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    initClouds();   // <-- crear las nubes en el cielo
    // --- Música de fondo ---
    musicPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(audioOutput);

    musicPlayer->setSource(QUrl::fromLocalFile(
        "C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/Lvl1S.mp3"
        ));

    audioOutput->setVolume(1);
    musicPlayer->setLoops(QMediaPlayer::Infinite);
}

void Level2Scene::onEnter()
{
    if (viewRef && player)
        viewRef->centerOn(player);
    connect(tickTimer, &QTimer::timeout, this, &Level2Scene::onTick);
    tickTimer->start(16);
    lastTimeMs = QDateTime::currentMSecsSinceEpoch();
    if (musicPlayer && musicPlayer->playbackState() != QMediaPlayer::PlayingState) {
        musicPlayer->play();
    }
}
void Level2Scene::onExit() {}

void Level2Scene::onTick()
{
    if (gameOver) return;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qreal dt = (now - lastTimeMs) / 1000.0;
    if (now - lastSecondTick >= 1000)
    {
        lastSecondTick = now;

        levelTime--;

        if (timerText)
            timerText->setPlainText(QString::number(levelTime));

        if (levelTime <= 0 && !gameOver)
        {
            triggerGameOver();
            return;
        }
    }
    lastTimeMs = now;

    if(player->health() <= 0 && !gameOver){
        triggerGameOver();
        return;
    }
    updateEntities(dt);
    updateHealthBar();
    updateHud();
    update();
    enemy2Shoot();
}

void Level2Scene::updateEntities(qreal dt)
{
    if (!player) return;

    // 1) Actualizar entidades y limitar al jugador, y saber si hay enemigos
    bool hayEnemigos = false;
    bool walkingForward = false;

    for (Entity* e : entities) {
        if (!e) continue;

        e->updateEntity(dt);

        if (e == player) {
            QPointF pos = player->pos();

            float minX = 200;
            float maxX = 880;
            float minY = 500;
            float maxY = 856;

            float halfW = player->pixmap().width() / 2.0f;
            float halfH = player->pixmap().height() / 2.0f;

            if (pos.x() < minX + halfW) pos.setX(minX + halfW);
            if (pos.x() > maxX - halfW) pos.setX(maxX - halfW);
            if (pos.y() < minY + halfH) pos.setY(minY + halfH);
            if (pos.y() > maxY - halfH) pos.setY(maxY - halfH);

            player->setPos(pos);

            if (player->up) {
                lastPlayerY += 1;
                walkingForward = true;
            }
        }

        if (dynamic_cast<Enemy2*>(e)) {
            hayEnemigos = true;
        }
    }

    // 2) Si no hay enemigos, revisar distancia recorrida para invocar horda
    if (!hayEnemigos) {
        travelledDistance = lastPlayerY;
        if (travelledDistance >= nextHordeAt) {
            spawnHorde();
        }
    }

    updateClouds(dt, walkingForward, hayEnemigos);

    // 3) Manejo de flechas (borra del vector de forma segura)
    for (int i = 0; i < static_cast<int>(entities.size()); ) {
        Arrow* arrow = dynamic_cast<Arrow*>(entities[i]);
        if (!arrow) {
            ++i;
            continue;
        }

        bool borrarFlecha = false;

        // 3.1 Flecha fuera de los límites verticales
        if ((arrow->direction == -1 && arrow->y() < 580) ||
            (arrow->direction == 1 && arrow->y() > 820)) {
            borrarFlecha = true;
        } else {
            // 3.2 Colisión flecha (jugador) -> enemigo (direction = -1)
            if (arrow->direction == -1) {
                for (int j = 0; j < static_cast<int>(entities.size()); ++j) {
                    Enemy2* enemy = dynamic_cast<Enemy2*>(entities[j]);
                    if (!enemy) continue;

                    if (arrow->collidesWithItem(enemy)) {
                        qDebug() << "Flecha golpeó enemigo";
                        enemy->takeDamage(arrow->getDamage());

                        if (enemy->getHealth() <= 0) {
                            removeItem(enemy);
                            delete enemy;
                            entities.erase(entities.begin() + j);

                            // si borramos antes de i, hay que ajustar
                            if (j < i) {
                                --i;
                            }
                        }

                        borrarFlecha = true;
                        break;
                    }
                }
            }

            // 3.3 Colisión flecha (enemigo) -> jugador (direction = 1)
            if (!borrarFlecha && arrow->direction == 1 && player &&
                arrow->collidesWithItem(player)) {

                if (!player->defen) {
                    player->takeDamage(arrow->getDamage());
                }
                borrarFlecha = true;
            }
        }

        if (borrarFlecha) {
            removeItem(arrow);
            delete arrow;
            entities.erase(entities.begin() + i);
        } else {
            ++i;
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
        int dir = -1;
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

    float hudX = topRight.x() - 220;
    float hudY = topRight.y() + 20;

    healthBack->setPos(hudX, hudY);
    healthBar->setPos(hudX, hudY);
    if (timerText)
    {
        QPointF topLeft = viewRef->mapToScene(viewRef->viewport()->rect().topLeft());
        timerText->setPos(topLeft.x() + 20, topLeft.y() + 20);
    }
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
    case 8:
        goToNextLevel();

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

    // 1) Primero, recopilar todos los enemigos en una lista aparte
    std::vector<Enemy2*> enemigos;
    enemigos.reserve(entities.size());

    for (Entity* e : entities) {
        Enemy2* enemy = dynamic_cast<Enemy2*>(e);
        if (enemy) {
            enemigos.push_back(enemy);
        }
    }

    // 2) Luego, iterar sobre esa lista y crear flechas SIN tocar el vector entities mientras lo recorremos
    for (Enemy2* enemy : enemigos) {
        int chance = QRandomGenerator::global()->bounded(1000);
        if (chance < 10) {
            QPointF start = enemy->pos();
            int direction = 1;

            Arrow* arrow = new Arrow(QPointF(start.x(), start.y() + 100),
                                     direction,
                                     150,
                                     10);
            addItem(arrow);
            entities.push_back(arrow); // aquí es seguro, ya no estamos iterando sobre entities
        }
    }
}

void Level2Scene::initClouds()
{
    cloudItems.clear();

    // Si no se cargó ninguna nube, no hacemos nada
    if (cloudSprite1.isNull() && cloudSprite2.isNull())
        return;

    int numClouds = 4;  // cuántas nubes quieres
    qreal sceneW = sceneRect().width();

    // Aquí guardamos las X ya usadas
    std::vector<qreal> usedX;
    usedX.reserve(numClouds);

    for (int i = 0; i < numClouds; ++i) {

        // Alternar entre cloud1 y cloud2
        QPixmap sprite;
        if (!cloudSprite1.isNull() && !cloudSprite2.isNull()) {
            sprite = (i % 2 == 0) ? cloudSprite1 : cloudSprite2;
        } else if (!cloudSprite1.isNull()) {
            sprite = cloudSprite1;
        } else {
            sprite = cloudSprite2;
        }

        if (sprite.isNull())
            continue;

        qreal x = 0;
        bool ok = false;
        const qreal minDist = 100.0;
        const int maxTries = 50;   // para que no se quede en bucle infinito

        for (int t = 0; t < maxTries && !ok; ++t) {
            x = QRandomGenerator::global()->bounded(static_cast<int>(sceneW));
            ok = true;

            for (qreal prevX : usedX) {
                if (qAbs(x - prevX) < minDist) {
                    ok = false;
                    break;
                }
            }
        }

        usedX.push_back(x);

        qreal y = 100 + QRandomGenerator::global()->bounded(120); // zona alta

        auto *cloud = new QGraphicsPixmapItem(sprite);
        cloud->setZValue(-90); // delante del fondo, detrás del player/HUD
        cloud->setPos(x, y);

        qreal scale = 0.8 + (QRandomGenerator::global()->bounded(40) / 100.0); // 0.8–1.2
        cloud->setScale(scale);

        addItem(cloud);
        cloudItems.push_back(cloud);
    }
}


void Level2Scene::updateClouds(qreal dt, bool walkingForward, bool hayEnemigos)
{
    if (cloudItems.empty())
        return;

    // Velocidad base de las nubes (para que no estén 100% quietas)
    const qreal baseSpeed = 10.0;    // px/s
    // Bonus de velocidad cuando el jugador camina hacia delante y no hay horda
    const qreal walkSpeed = 60.0;    // px/s

    qreal speed = baseSpeed;
    if (walkingForward && !hayEnemigos) {
        speed += walkSpeed;
    }

    qreal sceneW = sceneRect().width();

    for (QGraphicsPixmapItem *cloud : cloudItems) {
        if (!cloud) continue;

        QPointF pos = cloud->pos();
        pos.setX(pos.x() - speed * dt);

        // Si la nube sale por la izquierda, reaparece por la derecha
        if (pos.x() < -cloud->boundingRect().width()) {
            pos.setX(sceneW + QRandomGenerator::global()->bounded(100));
            pos.setY(40 + QRandomGenerator::global()->bounded(120));
        }

        cloud->setPos(pos);
    }
}

