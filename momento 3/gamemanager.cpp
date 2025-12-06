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
    connectLevel1Signals(level1Scene);   // <-- conectar señales aquí
    level2Scene = new Level2Scene(this);
    level3Scene = new Level3Scene(this);

    // Conexión del menú
    connect(menuScene, &MenuScene::startCampaignRequested, this, [this]() {
        // Siempre crear un nuevo Level1Scene al iniciar campaña
        level1Scene = new Level1Scene(this);
        connectLevel1Signals(level1Scene);   // <-- reconectar señales
        setGameScene(level1Scene);
    });

    // Conexiones de niveles 2 y 3
    connect(level2Scene, &Level2Scene::levelCompleted, this, [this]() {
        loadLevel3();
    });

    setGameScene(menuScene);
}

GameManager::~GameManager() {}

// Función auxiliar para conectar señales de Level1Scene
void GameManager::connectLevel1Signals(Level1Scene* scene) {
    connect(scene, &Level1Scene::gameOverOccurred, this, [this]() {
        handleGameOver();
    });
    connect(scene, &Level1Scene::levelCompleted, this, [this]() {
        loadLevel2();
    });
}

void GameManager::setGameScene(GameScene *scene) {
    if (!scene || scene == currentScene) return;

    if (currentScene) {
        currentScene->onExit(); // limpiar escena anterior
    }

    currentScene = scene;
    view->setScene(scene);

    if (auto l1 = dynamic_cast<Level1Scene*>(scene)) {
        l1->setView(view);
        l1->onEnter();
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

void GameManager::loadLevel2() {
    setGameScene(level2Scene);
}

void GameManager::loadLevel3() {
    setGameScene(level3Scene);
}

void GameManager::handleGameOver() {
    qDebug() << "Game Over Occurred - Handling Game Over...";
    // Regresar al menú después de 3 segundos
    QTimer::singleShot(3000, this, [this]() {
        setGameScene(menuScene);
    });
}
