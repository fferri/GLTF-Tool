#include "pageskin.h"
#include "mainwindow.h"
#include "ui_pageskin.h"
#include "tiny_gltf.h"
#include "gltfinfo.h"

PageSkin::PageSkin(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageSkin)
{
    ui->setupUi(this);
}

PageSkin::~PageSkin()
{
    delete ui;
}

void PageSkin::setData(const tinygltf::Model &model, int index)
{

}
