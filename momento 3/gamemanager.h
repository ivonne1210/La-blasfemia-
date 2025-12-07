#pragma once
#include <QMainWindow>
#include <QGraphicsView>
#include "gamescene.h"
#include "level2scene.h"
#include "level1scene.h"
#include "level3scene.h"
#include "CreditsScene.h"
#include "menuscene.h"

// gamemanager.h
class GameManager : public QMainWindow
{
    Q_OBJECT
public:
    explicit GameManager(QWidget *parent = nullptr);
    ~GameManager();

    void setGameScene(GameScene *scene);

public slots:
    void loadLevel2();
    void loadLevel3();
    void loadCredits();

private:
    void handleGameOverLevel1();
    void handleGameOverLevel2();
    void handleGameOverLevel3();
    void connectLevel1Signals(Level1Scene* scene);
    void connectLevel2Signals(Level2Scene* scene);
    void connectLevel3Signals(Level3Scene* scene);
    QGraphicsView *view;
    GameScene *currentScene = nullptr;
    MenuScene  *menuScene   = nullptr;
    Level1Scene *level1Scene = nullptr;
    Level2Scene *level2Scene = nullptr;
    Level3Scene *level3Scene = nullptr;
    CreditsScene *creditsScene = nullptr;
};
