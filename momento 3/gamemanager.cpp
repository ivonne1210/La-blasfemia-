#include "gamemanager.h"
#include "menuscene.h"
#include "level1scene.h"
#include <QVBoxLayout>

GameManager::GameManager(QWidget *parent)
    : QMainWindow(parent), view(new QGraphicsView(this)), currentScene(nullptr)
{
    setWindowTitle("La Blasfemia");
    resize(900, 700);

    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(view);

    // Start with menu
    MenuScene *menu = new MenuScene();
    connect(menu, &MenuScene::startCampaignRequested, this, [this](){
        // Start Level1 when menu requests campaign start
        Level1Scene *l1 = new Level1Scene();
        setGameScene(l1);
    });
    setGameScene(menu);
}

GameManager::~GameManager()
{
    if (currentScene) delete currentScene;
}

void GameManager::setGameScene(GameScene *scene)
{
    if (!scene) return;

    if (currentScene) {
        view->setScene(nullptr);
        delete currentScene;
    }

    currentScene = scene;
    view->setScene(scene);

    // Si es level 1 → le pasamos la vista y conectamos el "levelCompleted"
    if (auto l1 = dynamic_cast<Level1Scene*>(scene)) {
        l1->setView(view);

        // 🔥 CUANDO LEVEL1 Termine → cargamos LEVEL2
        connect(l1, &Level1Scene::levelCompleted, this, [this]() {
            loadLevel2();
        });
    }

    view->setFocus();
}

void GameManager::loadLevel2()
{
    // Aquí cargas tu escena del nivel 2
    Level2Scene *lvl2 = new Level2Scene();
    setGameScene(lvl2);
}
