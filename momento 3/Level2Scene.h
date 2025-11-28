#pragma once
#include "gamescene.h"
#include "player2.h"
#include "arrow.h"
#include "enemy2.h"
#include "item.h"
#include <QTimer>
#include <vector>
#include <QGraphicsView>
#include <QMediaPlayer>
#include <QAudioOutput>

class Level2Scene : public GameScene
{
    Q_OBJECT
signals:
    void levelCompleted();

public:
    explicit Level2Scene(QObject *parent = nullptr);
    QGraphicsView *viewRef = nullptr;
    ~Level2Scene() override;

    void onEnter() override;
    void onExit() override;
    void setView(QGraphicsView *v);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onTick();

private:
    void spawnArrow(int direction, QPointF pos);
    Player2 *player;
    QTimer *tickTimer;
    qint64 lastTimeMs;
    std::vector<Item*> items;
    QGraphicsPathItem* healthBack;
    QGraphicsPathItem* healthBar;

    std::vector<Entity*> entities;

    void setupScene();
    void spawnHorde();
    void updateEntities(qreal dt);
    void updateHealthBar();
    void updateHud();
    void enemy2Shoot();
    void spawnRandomEnemies(int starX, int starY);
    bool gameOver = false;
    QGraphicsTextItem* gameOverText = nullptr;
    QGraphicsPixmapItem* background;
    void triggerGameOver();
    bool portalActive = false;
    void goToNextLevel();
    QMediaPlayer *musicPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;
    float travelledDistance = 0.0f;
    float lastPlayerY = 0.0f;
    float nextHordeAt = 100.0f;
    int numeroH = 0;

};
