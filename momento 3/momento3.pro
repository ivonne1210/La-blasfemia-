TEMPLATE = app
CONFIG -= app_bundle
QT += core gui widgets
QT += multimedia

SOURCES += \
        Level2Scene.cpp \
        Level3Scene.cpp \
        actor.cpp \
        arrow.cpp \
        boomerangprojectile.cpp \
        boss3.cpp \
        enemy.cpp \
        enemy2.cpp \
        entity.cpp \
        gamemanager.cpp \
        gamescene.cpp \
        item.cpp \
        level1scene.cpp \
        main.cpp \
        menuscene.cpp \
        meteorprojectile.cpp \
        player.cpp \
        player2.cpp \
        player3.cpp

HEADERS += \
    Level2Scene.h \
    Level3Scene.h \
    actor.h \
    arrow.h \
    boomerangprojectile.h \
    boss3.h \
    enemy.h \
    enemy2.h \
    entity.h \
    gamemanager.h \
    gamescene.h \
    item.h \
    level1scene.h \
    menuscene.h \
    meteorprojectile.h \
    player.h \
    player2.h \
    player3.h

RESOURCES += \
    resources.qrc
