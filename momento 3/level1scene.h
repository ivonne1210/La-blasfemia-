#pragma once
#include "gamescene.h"
#include "player.h"
#include "item.h"
#include <QTimer>
#include <vector>
#include <QGraphicsView>
#include <QMediaPlayer>
#include <QAudioOutput>

class Level1Scene : public GameScene
{
    Q_OBJECT
signals:
    void levelCompleted();

public:
    explicit Level1Scene(QObject *parent = nullptr);
    QGraphicsView *viewRef = nullptr;
    ~Level1Scene() override;

    void onEnter() override;
    void onExit() override;
    void setView(QGraphicsView *v);

protected:
    // handle keyboard to control player
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onTick();

private:
    Player *player;
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
    void triggerGameOver();
    bool portalActive = false;
    QGraphicsPixmapItem* portalEffect = nullptr;
    void createPortalEffect();
    void goToNextLevel();
    QMediaPlayer *musicPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;


};
