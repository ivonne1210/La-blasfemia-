#pragma once
#include "gamescene.h"
#include "player2.h"
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
    Player2 *player;
    QTimer *tickTimer;
    qint64 lastTimeMs;
    std::vector<Item*> items;
    QGraphicsPathItem* healthBack;
    QGraphicsPathItem* healthBar;

    std::vector<Entity*> entities;

    void setupScene();
    void updateEntities(qreal dt);
    void updateHealthBar();
    void updateHud();
    void spawnRandomEnemies(int starX, int starY, int endX, int endY, int vel);
    bool gameOver = false;
    QGraphicsTextItem* gameOverText = nullptr;
    QGraphicsPixmapItem* background;
    void triggerGameOver();
    bool portalActive = false;
    void goToNextLevel();
    QMediaPlayer *musicPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;


};
