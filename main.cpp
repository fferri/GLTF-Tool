#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("fferri");
    QCoreApplication::setApplicationName("GLTF-Tool");
    MainWindow w;
    w.show();
    return a.exec();
}
