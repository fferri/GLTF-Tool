#include "pagetexture.h"
#include "mainwindow.h"
#include "ui_pagetexture.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageTexture::PageTexture(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageTexture)
{
    ui->setupUi(this);
}

PageTexture::~PageTexture()
{
    delete ui;
}

void PageTexture::setData(const tinygltf::Model &model, int index, int subIndex)
{

}
