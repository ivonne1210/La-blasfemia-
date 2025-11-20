#include <QApplication>
#include "gamemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameManager w;
    w.show();
    return a.exec();
}
