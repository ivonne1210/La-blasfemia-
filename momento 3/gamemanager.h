#pragma once
#include <QMainWindow>
#include <QGraphicsView>
#include "gamescene.h"
#include "level2scene.h"

class GameManager : public QMainWindow
{
    Q_OBJECT
public:
    explicit GameManager(QWidget *parent = nullptr);
    QGraphicsView* getView() const { return view;}
    ~GameManager() override;

    void setGameScene(GameScene *scene);

signals:
    void requestStartCampaign();

private:
    QGraphicsView *view;
    GameScene *currentScene;
    void loadLevel2();
};
