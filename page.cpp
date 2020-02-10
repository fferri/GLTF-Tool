#include "page.h"
#include "mainwindow.h"

Page::Page(MainWindow *mainWindow)
    : QWidget(mainWindow)
    , mainWindow_(mainWindow)
{

}

MainWindow * Page::mainWindow() const
{
    return mainWindow_;
}
