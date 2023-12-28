
#include "blockItem.h"
#include "paintArea.h"
#include "gameWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GameWidget gw;
    QIcon ico;
    ico.addFile("../resource/Tetris.png");
    gw.setWindowIcon(ico);
    gw.grabKeyboard();
    gw.show();
    return app.exec();
}
