#ifndef LEVEL3SCENE_H
#define LEVEL3SCENE_H

#include "gamescene.h"
#include "player3.h"
#include "boss3.h"
#include <QTimer>
#include <QVector>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPointF>

class QGraphicsView;
class Item;
class Entity;

class Level3Scene : public GameScene
{
    Q_OBJECT
signals:
    void finished();
    void gameOverOccurred();

public:
    explicit Level3Scene(QObject *parent = nullptr);
    ~Level3Scene() override;

    void onEnter() override;
    void onExit() override;

    void setView(QGraphicsView *v);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onTick();

private:
    void setupScene();
    void updateEntities(qreal dt);
    void handlePlatformCollisions(qint64 now);
    void updateHud();
     void handleBossPlayerCollisions(qreal dt);

    // --- Estado ---
    Player3 *player;
    Boss3 *boss = nullptr;
    QVector<Entity*> entities;    // jugador + enemigos, etc.
    QVector<Item*>   platforms;   // SOLO plataformas (tipo Item)

    QGraphicsView *viewRef = nullptr;

    QTimer *tickTimer = nullptr;
    qint64 lastTimeMs = 0;
    bool gameOver = false;

    // Música
    QMediaPlayer *musicPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;

    // Plataformas de una vía
    QPointF lastPlayerPos;        // posición del frame anterior
    bool downPressed = false;
    qint64 dropThroughUntilMs = 0; // tiempo hasta el que ignoramos plataformas
    bool inicio = true;

    // vida
    QGraphicsPathItem* healthBack;
    QGraphicsPathItem* healthBar;

    QGraphicsRectItem *bossHpBg = nullptr;
    QGraphicsRectItem *bossHpFg = nullptr;
    qreal bossHpWidth = 0.0;

    bool  playerStuckToBoss    = false;
    qreal stuckOffsetX         = 0.0;
    qreal stuckOffsetY         = 0.0;

    qint64 lastBossHitByPlayerMs = 0;   // cooldown para golpes de espada

    void triggerGameOver();
    void goToFinish();
    QGraphicsTextItem* gameOverText = nullptr;
};

#endif // LEVEL3SCENE_H
