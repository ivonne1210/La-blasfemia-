#ifndef LEVEL3SCENE_H
#define LEVEL3SCENE_H

#include "gamescene.h"
#include "player3.h"
#include <QTimer>
#include <QVector>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPointF>

class QGraphicsView;
class Item;
class Entity;

class Level3Scene : public GameScene
{
    Q_OBJECT
public:
    explicit Level3Scene(QObject *parent = nullptr);
    ~Level3Scene() override;

    void onEnter() override;
    void onExit() override;

    void setView(QGraphicsView *v);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onTick();

private:
    void setupScene();
    void updateEntities(qreal dt);
    void handlePlatformCollisions(qint64 now);
    void updateHud();

    // --- Estado ---
    Player3 *player;
    QVector<Entity*> entities;    // jugador + enemigos, etc.
    QVector<Item*>   platforms;   // SOLO plataformas (tipo Item)

    QGraphicsView *viewRef = nullptr;

    QTimer *tickTimer = nullptr;
    qint64 lastTimeMs = 0;
    bool gameOver = false;

    // Música
    QMediaPlayer *musicPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;

    // Plataformas de una vía
    QPointF lastPlayerPos;        // posición del frame anterior
    bool downPressed = false;
    qint64 dropThroughUntilMs = 0; // tiempo hasta el que ignoramos plataformas
    bool inicio = true;

};

#endif // LEVEL3SCENE_H
