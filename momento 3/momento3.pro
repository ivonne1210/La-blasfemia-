TEMPLATE = app
CONFIG -= app_bundle
QT += core gui widgets
QT += multimedia

SOURCES += \
        Level2Scene.cpp \
        actor.cpp \
        arrow.cpp \
        enemy.cpp \
        enemy2.cpp \
        entity.cpp \
        gamemanager.cpp \
        gamescene.cpp \
        item.cpp \
        level1scene.cpp \
        main.cpp \
        menuscene.cpp \
        player.cpp \
        player2.cpp

HEADERS += \
    Level2Scene.h \
    actor.h \
    arrow.h \
    enemy.h \
    enemy2.h \
    entity.h \
    gamemanager.h \
    gamescene.h \
    item.h \
    level1scene.h \
    menuscene.h \
    player.h \
    player2.h

RESOURCES += \
    resources.qrc
