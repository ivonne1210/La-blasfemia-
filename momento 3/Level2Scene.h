#ifndef LEVEL2SCENE_H
#define LEVEL2SCENE_H

#include "gamescene.h"
#include <QGraphicsTextItem>
#include <QTimer>

class Level2Scene : public GameScene
{
    Q_OBJECT
public:
    explicit Level2Scene(QObject *parent = nullptr);
    ~Level2Scene();

    void onEnter() override {}
    void onExit() override {}

private:
    QGraphicsTextItem *label;
};

#endif // LEVEL2SCENE_H
