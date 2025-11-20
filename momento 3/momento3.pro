TEMPLATE = app
CONFIG -= app_bundle
QT += core gui widgets
QT += multimedia

SOURCES += \
        actor.cpp \
        entity.cpp \
        gamemanager.cpp \
        gamescene.cpp \
        level1scene.cpp \
        main.cpp \
        menuscene.cpp \
        player.cpp

HEADERS += \
    actor.h \
    entity.h \
    gamemanager.h \
    gamescene.h \
    level1scene.h \
    menuscene.h \
    player.h

RESOURCES += \
    resources.qrc
