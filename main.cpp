#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "videoencode.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    VideoEncode();
    return a.exec();
}
