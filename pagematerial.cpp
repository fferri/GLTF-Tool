#include "pagematerial.h"
#include "mainwindow.h"
#include "ui_pagematerial.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageMaterial::PageMaterial(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageMaterial)
{
    ui->setupUi(this);
}

PageMaterial::~PageMaterial()
{
    delete ui;
}

void PageMaterial::setData(const tinygltf::Model &model, int index)
{

}
