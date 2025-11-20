#pragma once
#include <QGraphicsScene>

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = nullptr);
    virtual ~GameScene() override = default;

    // lifecycle hooks
    virtual void onEnter() {}
    virtual void onExit() {}
};
