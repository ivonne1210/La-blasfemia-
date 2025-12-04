#pragma once
#include "gamescene.h"
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class MenuScene : public GameScene
{
    Q_OBJECT
public:
    explicit MenuScene(QObject *parent = nullptr);
    void onEnter() override;
    void onExit() override;

private:
    QMediaPlayer *musicPlayer;
    QAudioOutput *audioOutput;

signals:
    void startCampaignRequested();
};
