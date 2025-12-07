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

    QPixmap bg("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/back3.png");
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

    boss = new Boss3(player);
    boss->setPos(700, 780);
    boss->setGroundY(750);
    addItem(boss);
    entities.push_back(boss);


    const qreal PLATFORM_SCALE = 0.28;
    const qreal PLATFORM_SCALE1 = 0.1;

    Item *plat2 = new Item("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/platform.png");
    plat2->setScale(PLATFORM_SCALE);
    plat2->setPos(100, 600);
    addItem(plat2);
    plat2->setZValue(-99);
    platforms.push_back(plat2);

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
        "C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/Lvl3S.mp3"
        ));
    audioOutput->setVolume(0.8);
    musicPlayer->setLoops(QMediaPlayer::Infinite);

    //Vida
    QPainterPath backPath;
    backPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBack = addPath(backPath, QPen(Qt::black), QBrush(Qt::darkGray));
    healthBack->setZValue(1000);
    healthBack->setPos(690,210);

    QPainterPath barPath;
    barPath.addRoundedRect(0, 0, 200, 20, 8, 8);

    healthBar = addPath(barPath, QPen(Qt::NoPen), QBrush(QColor(132,41,30)));
    healthBar->setZValue(1001);
    healthBar->setPos(690,210);

    // ===== BARRA DE VIDA DEL BOSS ABAJO =====
    bossHpWidth = SCENE_W - 80;
    qreal barHeight     = 14.0;
    qreal marginSide    = 40.0;
    qreal marginBottom  = 30.0;

    qreal x = marginSide;
    qreal y = SCENE_H - barHeight - marginBottom;

    bossHpBg = addRect(
        x, y,
        bossHpWidth, barHeight,
        QPen(Qt::NoPen),
        QBrush(QColor(15, 0, 0, 220))
        );

    // Barra roja “sangre”
    QLinearGradient grad(x, y, x, y + barHeight);
    grad.setColorAt(0.0, QColor(80, 0, 0));
    grad.setColorAt(0.5, QColor(210, 20, 20));
    grad.setColorAt(1.0, QColor(120, 0, 0));

    bossHpFg = addRect(
        x, y,
        bossHpWidth, barHeight,
        QPen(Qt::NoPen),
        QBrush(grad)
        );

    bossHpBg->setZValue(1000);
    bossHpFg->setZValue(1001);
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
    handleBossPlayerCollisions(dt);
    updateHud();
    update();

    if(player->health()<=0){
        triggerGameOver();
    }
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

void Level3Scene::handlePlatformCollisions(qint64 now)
{
    if (!player) return;
    if (platforms.isEmpty()) return;
    player->setPlat(false);
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
        player->setPlat(true);
    }
}


void Level3Scene::updateHud()
{

    if (player) {
        qreal playerHealthRatio = static_cast<qreal>(player->health()) / 100.0; // 100 es el valor máximo de vida

        // Crear la barra de salud de fondo (si no está actualizada)
        QPainterPath healthBgPath;
        healthBgPath.addRoundedRect(0, 0, 200, 20, 5, 5);  // Barra de fondo con un tamaño fijo
        healthBack->setPath(healthBgPath);  // Actualizamos la barra de fondo (background)

        // Crear la barra de salud visible usando QPainterPath
        QPainterPath healthPath;
        healthPath.addRoundedRect(0, 0, 200 * playerHealthRatio, 20, 5, 5);  // 100 es el ancho máximo de la barra

        // Actualizar la barra de salud visible (foreground)
        healthBar->setPath(healthPath);  // Actualizamos la barra de salud del jugador
    }

    if (!boss || !bossHpFg)
        return;

    int hp  = boss->health();
    int max = boss->maxHealth();
    if (max <= 0) max = 1;

    qreal ratio = static_cast<qreal>(hp) / static_cast<qreal>(max);
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;

    QRectF r = bossHpFg->rect();
    r.setWidth(bossHpWidth * ratio);   // encoger barra
    bossHpFg->setRect(r);

    if (boss->health()<=0){
        goToFinish();
    }
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

    case Qt::Key_K:
        player->setGuard(true);
        break;

    case Qt::Key_L:
        player->setAttack(true);
        break;


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

    case Qt::Key_K:
        player->setGuard(false);
        break;

    case Qt::Key_L:
        player->setAttack(false);
        break;

    default:
        QGraphicsScene::keyReleaseEvent(event);
    }
}

void Level3Scene::handleBossPlayerCollisions(qreal /*dt*/)
{
    if (!player || !boss) return;

    // 1) Ataque del jugador -> daño al boss
    if (player->isAttacking() && player->collidesWithItem(boss)) {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        // para que no haga 60 golpes por segundo
        if (now - lastBossHitByPlayerMs > 1000) {   // 0.25 s de cooldown
            boss->applyDamage(15);
            lastBossHitByPlayerMs = now;
        }
    }

    // 2) Colisión inelástica cuando el boss hace salto de ataque
    if (playerStuckToBoss) {
        // Mientras esté "pegado", seguir al boss con un offset fijo
        QPointF bp = boss->pos();
        player->setPos(bp.x() + stuckOffsetX,
                       bp.y() + stuckOffsetY);

        // Cuando el boss pisa el suelo, soltamos y hacemos daño
        if (boss->isOnGround()) {
            playerStuckToBoss = false;

            // lo soltamos a un lado del boss
            qreal side = (player->x() < boss->x()) ? -1.0 : 1.0;
            QPointF releasePos = boss->pos();
            releasePos.setX(releasePos.x() + side * 80.0);
            player->setPos(releasePos);

            int hp = player->health();
            hp -= 20;                 // ajusta al gusto
            if (hp < 0) hp = 0;{
            }
            player->setHealth(hp);
        }
        return; // mientras esté pegado, no chequeamos nueva colisión
    }

    // Si aún no está pegado, miramos si el boss está en salto de ataque
    if (boss->isJumpAttacking() && player->collidesWithItem(boss)) {
        playerStuckToBoss = true;

        // offset relativo: ligeramente a un lado y un poco arriba
        qreal side = (player->x() < boss->x()) ? -1.0 : 1.0;
        stuckOffsetX = side * 30.0;
        stuckOffsetY = -20.0;
    }
}

void Level3Scene::triggerGameOver()
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

    emit gameOverOccurred();
}

void Level3Scene::goToFinish(){
    emit finished();
}

