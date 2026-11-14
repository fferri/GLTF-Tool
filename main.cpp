#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("fferri");
    QCoreApplication::setApplicationName("GLTF-Tool");
    a.setWindowIcon(QIcon(":/gltftool.png"));
    MainWindow w;
    w.show();
    return a.exec();
}
