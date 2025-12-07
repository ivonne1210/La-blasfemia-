#include "CreditsScene.h"
#include <QGraphicsTextItem>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QBrush>
#include <QColor>

CreditsScene::CreditsScene(QObject *parent)
    : GameScene(parent)
{
    setupCredits();
    connectSignals();
}

CreditsScene::~CreditsScene() {}

void CreditsScene::setupCredits()
{
    // Cargar la imagen de fondo
    QPixmap backgroundPixmap("C:/Users/kevin/OneDrive/Escritorio/info2/la blasfemia/momento 2/sprites/creditos.png");

    if (!backgroundPixmap.isNull()) {
        // Escalar la imagen de fondo para que se ajuste al tamaño de la escena
        QSizeF sceneSize(800, 600);  // Definir un tamaño fijo para la escena
        QPixmap scaledBackground = backgroundPixmap.scaled(sceneSize.toSize(),
                                                           Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        QBrush brush(scaledBackground);
        setBackgroundBrush(brush);
    } else {
        // Si la imagen no se carga, poner un fondo negro
        setBackgroundBrush(Qt::black);
    }

    // Crear el texto de los créditos
    creditsText = new QGraphicsTextItem("Créditos:\n\n"
                                        "Desarrollado por:\n"
                                        "Kevin Esteban Echeverri\n"
                                        "Ivonne Lizeth Rosero\n\n"
                                        "Música :\n"
                                        ".Goëtia\n"
                                        "Chopin: Nocturne in c sharp\n"
                                        "Carl Orff - O Fortuna ~ Carmina Burana\n\n"
                                        "Arte por:\n"
                                        "Kevin Esteban Echeverri\n\n"
                                        "Consultor artistico:\n"
                                        "Daniel Marin\n\n"
                                        "¡Gracias por jugar!");
    creditsText->setDefaultTextColor(QColor(255, 255, 255));
    creditsText->setFont(QFont("Old English Text MT", 20, QFont::Bold));
    creditsText->setPos(0, 0);  // Posición inicial
    addItem(creditsText);

    frase = new QGraphicsTextItem("La gracia de\n" "nuestro señor\n" "jesucristo\n" "sea con todos\n" "vosotros");
    frase->setDefaultTextColor(QColor(132, 41, 30));
    frase->setFont(QFont("Old English Text MT", 24, QFont::Bold));
    frase->setPos(600, 0);  // Posición inicial
    addItem(frase);

    // Crear el botón de regreso
    backButton = new QPushButton("Regresar al Menú");
    backButton->setGeometry(350, 500, 200, 50);
    backButton->setStyleSheet("background-color: #333; color: white; font-size: 18px;");

    // Posicionar el botón dentro de la escena, asegurándose de que esté visible
    QGraphicsProxyWidget *buttonWidget = addWidget(backButton);
    buttonWidget->setPos(625, 550);  // Ajusté la posición para que el botón esté dentro del área visible

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

    backButton->setFont(buttonFont);
    backButton->setStyleSheet(buttonStyle);
}

void CreditsScene::connectSignals()
{
    // Conectar el botón de regreso para que regrese al menú
    connect(backButton, &QPushButton::clicked, this, [this]() {
        // Regresar al menú principal
        emit returnToMenu(); // Emite una señal para que el GameManager maneje el cambio de escena
    });
}
