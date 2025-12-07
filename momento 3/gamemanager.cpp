#include "gamemanager.h"
#include "menuscene.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>

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

    // Escenas iniciales
    menuScene = new MenuScene(this);

    level1Scene = new Level1Scene(this);
    connectLevel1Signals(level1Scene);

    level2Scene = new Level2Scene(this);
    connectLevel2Signals(level2Scene);

    level3Scene = new Level3Scene(this);

    // Conexión del menú
    connect(menuScene, &MenuScene::startCampaignRequested, this, [this]() {
        // Siempre crear un nuevo Level1Scene al iniciar campaña
        level1Scene = new Level1Scene(this);
        connectLevel1Signals(level1Scene);
        setGameScene(level1Scene);
    });

    setGameScene(menuScene);
}

GameManager::~GameManager() {}

//
// Conexiones de Level1
//
void GameManager::connectLevel1Signals(Level1Scene* scene) {
    connect(scene, &Level1Scene::gameOverOccurred, this, [this]() {
        handleGameOverLevel1();
    });
    connect(scene, &Level1Scene::levelCompleted, this, [this]() {
        loadLevel2();
    });
}

//
// Conexiones de Level2
//
void GameManager::connectLevel2Signals(Level2Scene* scene) {
    connect(scene, &Level2Scene::gameOverOccurred, this, [this]() {
        handleGameOverLevel2();
    });
    connect(scene, &Level2Scene::levelCompleted, this, [this]() {
        loadLevel3();
    });
}

void GameManager::connectLevel3Signals(Level3Scene* scene) {
    connect(scene, &Level3Scene::gameOverOccurred, this, [this]() {
        handleGameOverLevel3();
    });
    connect(scene, &Level3Scene::finished, this, [this]() {
        //creditos
        loadCredits();
    });
}

//
// Cambio de escena
//
void GameManager::setGameScene(GameScene *scene) {
    if (!scene || scene == currentScene) return;

    if (currentScene) {
        currentScene->onExit();
    }

    currentScene = scene;
    view->setScene(scene);

    if (auto l1 = dynamic_cast<Level1Scene*>(scene)) {
        l1->setView(view);
        l1->onEnter();
    }
    if (auto l2 = dynamic_cast<Level2Scene*>(scene)) {
        l2->setView(view);
        l2->onEnter();
    }
    if (auto l3 = dynamic_cast<Level3Scene*>(scene)) {
        l3->setView(view);
        l3->onEnter();
    }

    view->setFocus();
}

//
// Cargar niveles
//
void GameManager::loadLevel2() {
    level2Scene = new Level2Scene(this);
    connectLevel2Signals(level2Scene);
    setGameScene(level2Scene);
}

void GameManager::loadLevel3() {
    level3Scene = new Level3Scene(this);
    connectLevel3Signals(level3Scene);
    setGameScene(level3Scene);
}

void GameManager::handleGameOverLevel1() {
    QTimer::singleShot(3000, this, [this]() {
        setGameScene(menuScene);
    });
}

void GameManager::handleGameOverLevel2() {
    QTimer::singleShot(3000, this, [this]() {
        level1Scene = new Level1Scene(this);
        connectLevel1Signals(level1Scene);

        setGameScene(menuScene);
    });
}

void GameManager::handleGameOverLevel3() {
    QTimer::singleShot(3000, this, [this]() {
        level1Scene = new Level1Scene(this);
        connectLevel1Signals(level1Scene);

        setGameScene(menuScene);
    });
}

void GameManager::loadCredits()
{
    creditsScene = new CreditsScene(this);
    connect(creditsScene, &CreditsScene::returnToMenu, this, [this]() {
        setGameScene(menuScene);
    });
    setGameScene(creditsScene);
}



