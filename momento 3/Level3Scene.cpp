#include "level3scene.h"
#include "player2.h"      // ⬅️ cambia si usas otra clase
#include "item.h"
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QDateTime>
#include <QDebug>

Level3Scene::Level3Scene(QObject *parent)
    : GameScene(parent)
{
    // Timer principal del nivel
    tickTimer = new QTimer(this);
    connect(tickTimer, &QTimer::timeout, this, &Level3Scene::onTick);

    setupScene();
}

Level3Scene::~Level3Scene()
{
    if (tickTimer) tickTimer->stop();
}

void Level3Scene::setupScene()
{
    // === BACKGROUND ===
    const int SCENE_W = 900;
    const int SCENE_H = 900;
    setSceneRect(0, 0, SCENE_W, SCENE_H);

    QPixmap bg("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/back3.png"); // ⬅️ pon tu ruta
    if (bg.isNull()) {
        setBackgroundBrush(Qt::black);
    } else {
        // Escalamos el fondo al tamaño de la escena
        QPixmap bgScaled = bg.scaled(
            SCENE_W,
            SCENE_H,
            Qt::IgnoreAspectRatio,
            Qt::FastTransformation
            );

        auto *bgItem = new QGraphicsPixmapItem(bgScaled);
        bgItem->setZValue(-100);
        bgItem->setPos(0, 0);
        addItem(bgItem);
    }

    player = new Player3();
    addItem(player);
    player->setPos(50, 600);
    entities.push_back(player);

    const qreal PLATFORM_SCALE = 0.28;
    const qreal PLATFORM_SCALE1 = 0.1;

    // Plataforma 2
    Item *plat2 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/platform.png");
    plat2->setScale(PLATFORM_SCALE);
    plat2->setPos(100, 600);
    addItem(plat2);
    plat2->setZValue(-99);
    platforms.push_back(plat2);

    // Plataforma 3
    Item *plat3 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/platform1.png");
    plat3->setScale(PLATFORM_SCALE1);
    plat3->setPos(425, 675);
    addItem(plat3);
    plat3->setZValue(-99);
    platforms.push_back(plat3);

    Item *plat4 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/platform.png");
    plat4->setScale(PLATFORM_SCALE);
    plat4->setPos(625, 600);
    addItem(plat4);
    plat4->setZValue(-99);
    platforms.push_back(plat4);

    Item *plat5 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/platform.png");
    plat5->setScale(PLATFORM_SCALE);
    plat5->setPos(365, 450);
    addItem(plat5);
    plat5->setZValue(-99);
    platforms.push_back(plat5);

    Item *plat6 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/platform1.png");
    plat6->setScale(PLATFORM_SCALE1);
    plat6->setPos(150, 400);
    addItem(plat6);
    plat6->setZValue(-99);
    platforms.push_back(plat6);

    Item *plat7 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/platform1.png");
    plat7->setScale(PLATFORM_SCALE1);
    plat7->setPos(675, 400);
    addItem(plat7);
    plat7->setZValue(-99);
    platforms.push_back(plat7);

    // === MÚSICA ===
    musicPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(audioOutput);

    musicPlayer->setSource(QUrl::fromLocalFile(
        "C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/Lvl1S.mp3"    // ⬅️ cambia ruta
        ));
    audioOutput->setVolume(0.8);
    musicPlayer->setLoops(QMediaPlayer::Infinite);
    // NO reproducimos aquí, solo en onEnter()
}

void Level3Scene::setView(QGraphicsView *v)
{
    viewRef = v;
}

void Level3Scene::onEnter()
{
    gameOver = false;
    lastTimeMs = QDateTime::currentMSecsSinceEpoch();
    if (tickTimer && !tickTimer->isActive()) {
        tickTimer->start(16);   // ~60 FPS
    }

    if (musicPlayer &&
        musicPlayer->playbackState() != QMediaPlayer::PlayingState) {
        musicPlayer->play();
    }
}

void Level3Scene::onExit()
{
    if (tickTimer) tickTimer->stop();
    if (musicPlayer) musicPlayer->stop();
}

void Level3Scene::onTick()
{
    if (gameOver) return;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qreal dt = (now - lastTimeMs) / 1000.0;
    lastTimeMs = now;

    if (player) {
        lastPlayerPos = player->pos();   // guardar posición anterior
    }

    // Centrar cámara en el jugador si quieres
    if (inicio) {
        viewRef->centerOn(425, 625);
        inicio = false;
    }


    updateEntities(dt);
    handlePlatformCollisions(now);
    updateHud();
    update();
}

void Level3Scene::updateEntities(qreal dt)
{
    // Aquí actualizas al jugador y demás entidades
    for (Entity *e : entities) {
        if (!e) continue;
        e->updateEntity(dt);   // misma idea que en Level1 / Level2
    }

    // Puedes limitar el movimiento del jugador a los bordes de la escena
    if (player) {
        QPixmap pm = player->pixmap();
        qreal halfW = pm.width() / 2.0;
        qreal halfH = pm.height() / 2.0;

        QPointF pos = player->pos();
        QRectF bounds(sceneRect());

        if (pos.x() < bounds.left() + halfW)  pos.setX(bounds.left() + halfW);
        if (pos.x() > bounds.right() - halfW) pos.setX(bounds.right() - halfW);
        if (pos.y() < bounds.top() + halfH)   pos.setY(bounds.top() + halfH);
        if (pos.y() > bounds.bottom() - halfH)pos.setY(bounds.bottom() - halfH);

        player->setPos(pos);
    }
}

// ------- PLATAFORMAS DE UNA VÍA -------
// - Si el jugador va hacia ARRIBA: no colisiona nunca con las plataformas.
// - Si va hacia ABAJO (cayendo) y cruza el borde superior de una plataforma
//   y no está en modo "dropThrough", lo pegamos encima de la plataforma.
// - Si mantiene S o flecha abajo: activamos "dropThrough" un ratito y no
//   aplicamos la colisión de plataformas.

void Level3Scene::handlePlatformCollisions(qint64 now)
{
    if (!player) return;
    if (platforms.isEmpty()) return;
    bool ignorePlatforms = (now <= dropThroughUntilMs);

    QPixmap pm = player->pixmap();
    qreal halfW = pm.width() / 2.0;
    qreal halfH = pm.height() / 4.0;
    qreal playerWidth = pm.width();

    QPointF prevPos = lastPlayerPos;
    QPointF currPos = player->pos();

    bool falling = (currPos.y() > prevPos.y());
    if (!falling || ignorePlatforms) {
        return;
    }

    qreal bottomPrev = prevPos.y() + halfH;
    qreal bottomNow  = currPos.y() + halfH;

    Item *bestPlatform = nullptr;
    qreal bestPlatformTop = 0.0;

    for (Item *plat : platforms) {
        if (!plat) continue;

        QRectF platRect = plat->sceneBoundingRect();

        qreal playerLeft  = currPos.x() - halfW;
        qreal playerRight = currPos.x() + halfW;

        qreal overlapLeft  = qMax(playerLeft,  platRect.left());
        qreal overlapRight = qMin(playerRight, platRect.right());
        qreal overlapWidth = overlapRight - overlapLeft;

        if (overlapWidth <= 0) continue;

        if (overlapWidth < playerWidth * 0.5)
            continue;

        bool wasAbove    = (bottomPrev <= platRect.top());
        bool nowBelowTop = (bottomNow  >= platRect.top());

        if (wasAbove && nowBelowTop) {
            if (!bestPlatform || platRect.top() < bestPlatformTop) {
                bestPlatform    = plat;
                bestPlatformTop = platRect.top();
            }
        }
    }

    if (bestPlatform) {
        QPointF snapPos = currPos;
        snapPos.setY(bestPlatformTop - halfH );
        player->setPos(snapPos);

        player->setVerticalVelocity(0.0);
    }
}


void Level3Scene::updateHud()
{
}


void Level3Scene::keyPressEvent(QKeyEvent *event)
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
        player->jump(true);
        break;

    case Qt::Key_Down:
    case Qt::Key_S: {
        downPressed = true;
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        dropThroughUntilMs = now + 250;
        break;
    }

    default:
        QGraphicsScene::keyPressEvent(event);
    }
}

void Level3Scene::keyReleaseEvent(QKeyEvent *event)
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
        player->jump(false);
        break;

    case Qt::Key_Down:
    case Qt::Key_S:
        downPressed = false;
        break;

    default:
        QGraphicsScene::keyReleaseEvent(event);
    }
}
