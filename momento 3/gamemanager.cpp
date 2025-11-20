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
    MenuScene *menu = new MenuScene(this);
    connect(menu, &MenuScene::startCampaignRequested, this, [this](){
        // Start Level1 when menu requests campaign start
        Level1Scene *l1 = new Level1Scene(this);
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
    // Give focus to view so scenes receive key events
    view->setFocus();
}
