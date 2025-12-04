#include "gamemanager.h"
#include "menuscene.h"
#include <QVBoxLayout>

GameManager::GameManager(QWidget *parent)
    : QMainWindow(parent),
    view(new QGraphicsView(this)),
    currentScene(nullptr)
{
    setWindowTitle("La Blasfemia");
    resize(900, 700);

    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(view);

    menuScene   = new MenuScene(this);
    level1Scene = new Level1Scene(this);
    level2Scene = new Level2Scene(this);
    level3Scene = new Level3Scene(this);

    connect(menuScene, &MenuScene::startCampaignRequested, this, [this]() {
        //setGameScene(level1Scene);
        loadLevel3();
    });

    connect(level1Scene, &Level1Scene::levelCompleted, this, [this]() {
        loadLevel3();
    });

    setGameScene(menuScene);
}

GameManager::~GameManager()
{
}

void GameManager::setGameScene(GameScene *scene)
{
    if (!scene || scene == currentScene)
        return;

    if (currentScene) {
        currentScene->onExit();
    }

    currentScene = scene;
    view->setScene(scene);

    if (auto l1 = dynamic_cast<Level1Scene*>(scene)) {
        l1->setView(view);
    }

    if (auto lvl2 = dynamic_cast<Level2Scene*>(scene)) {
        lvl2->setView(view);
        lvl2->onEnter();
    }

    if (auto lvl3 = dynamic_cast<Level3Scene*>(scene)) {
        lvl3->setView(view);
        lvl3->onEnter();
    }

    view->setFocus();
}

void GameManager::loadLevel2()
{
    setGameScene(level2Scene);
}

void GameManager::loadLevel3()
{
    setGameScene(level3Scene);
}
