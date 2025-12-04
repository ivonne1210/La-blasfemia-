#include "menuscene.h"
#include <QGraphicsTextItem>
#include <QApplication>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QFont>
#include <QPixmap>
#include <QBrush>


MenuScene::MenuScene(QObject *parent)
    : GameScene(parent)
{
    setSceneRect(0,0,800,600);

    QPixmap backgroundPixmap("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/menu.png");
    if (!backgroundPixmap.isNull()) {

        QSizeF sceneSize = sceneRect().size();

        QPixmap scaledBackground = backgroundPixmap.scaled(
            sceneSize.toSize(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
            );

        QBrush brush(scaledBackground);
        setBackgroundBrush(brush);
    } else {
        setBackgroundBrush(Qt::black);
    }

    QPixmap titlePixmap("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/title.png");

    if (!titlePixmap.isNull()) {
        // Escalar la imagen: ancho 400 px, altura proporcional
        QPixmap scaledTitle = titlePixmap.scaled(
            400,                        // ancho deseado
            titlePixmap.height(),       // altura máxima (se ajusta automáticamente)
            Qt::KeepAspectRatio,        // mantiene proporciones
            Qt::SmoothTransformation    // suaviza al redimensionar
            );

        QGraphicsPixmapItem* titleItem = addPixmap(scaledTitle);
        titleItem->setPos(280, 400);
    }

    QPushButton *startBtn = new QPushButton("Iniciar Campaña");
    startBtn->setFixedSize(200, 40);
    QGraphicsProxyWidget *pStart = addWidget(startBtn);
    pStart->setPos(0, 400);
    connect(startBtn, &QPushButton::clicked, this, [this](){
        emit startCampaignRequested();
    });

    QPushButton *optionsBtn = new QPushButton("Opciones");
    optionsBtn->setFixedSize(220, 40);
    QGraphicsProxyWidget *pOpt = addWidget(optionsBtn);
    pOpt->setPos(0, 450);

    QPushButton *exitBtn = new QPushButton("Salir");
    exitBtn->setFixedSize(220, 40);
    QGraphicsProxyWidget *pExit = addWidget(exitBtn);
    pExit->setPos(0, 500);
    connect(exitBtn, &QPushButton::clicked, this, [](){
        qApp->quit();
    });

    QFont buttonFont("Old English Text MT");
    buttonFont.setPointSize(16);
    buttonFont.setBold(true);

    // --- Color de texto y fondo usando StyleSheet ---
    QString buttonStyle = "QPushButton { "
                          "color: rgb(132,41,30);"   // color del texto (rojo)
                          "background-color: rgb(0,0,0);" // fondo gris oscuro
                          "border: 2px solid rgb(0,0,0);" // borde rojo
                          "border-radius: 5px;"
                          "text-align: left;"
                          "} "
                          "QPushButton::hover { "
                          "background-color: rgb(50,50,50);"   // color al pasar mouse
                          "}";

    startBtn->setFont(buttonFont);
    startBtn->setStyleSheet(buttonStyle);

    exitBtn->setFont(buttonFont);
    exitBtn->setStyleSheet(buttonStyle);

    optionsBtn->setFont(buttonFont);
    optionsBtn->setStyleSheet(buttonStyle);

    // ---- MÚSICA DE FONDO ----
    musicPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(audioOutput);

    // Ruta del archivo MP3
    musicPlayer->setSource(QUrl::fromLocalFile(
        "C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/menuS.mp3"
        ));

    // Volumen (0.0 a 1.0)
    audioOutput->setVolume(0.5);

    // Reproducir en loop infinito (Qt 6.5+)
    musicPlayer->setLoops(QMediaPlayer::Infinite);

    // Iniciar la reproducción
    musicPlayer->play();

}

void MenuScene::onExit()
{
    if (musicPlayer) {
        musicPlayer->stop();
    }
}

void MenuScene::onEnter()
{
    if (musicPlayer && musicPlayer->playbackState() != QMediaPlayer::PlayingState) {
        musicPlayer->play();
    }
}
