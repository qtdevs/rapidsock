#include <QApplication>
#include "serverwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);
    ServerWindow w;
    w.show();

    return a.exec();
}
