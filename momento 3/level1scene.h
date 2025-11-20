#pragma once
#include "gamescene.h"
#include "player.h"
#include <QTimer>
#include <vector>

class Level1Scene : public GameScene
{
    Q_OBJECT
public:
    explicit Level1Scene(QObject *parent = nullptr);
    ~Level1Scene() override;

    void onEnter() override;
    void onExit() override;

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

    // store entities for update loop
    std::vector<Entity*> entities;

    void setupScene();
    void updateEntities(qreal dt);
};
