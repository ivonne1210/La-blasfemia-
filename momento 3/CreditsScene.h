#ifndef CREDITS_SCENE_H
#define CREDITS_SCENE_H

#include "gamescene.h"  // Si estás usando una clase base GameScene
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QPushButton>

class CreditsScene : public GameScene
{
    Q_OBJECT
public:
    explicit CreditsScene(QObject *parent = nullptr);
    ~CreditsScene();

signals:
    void returnToMenu();

private:
    QGraphicsTextItem* creditsText;
    QGraphicsTextItem* frase;
    QPushButton* backButton;

    void setupCredits();
    void connectSignals();
};

#endif // CREDITS_SCENE_H
