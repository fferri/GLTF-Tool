#include "pagelight.h"
#include "mainwindow.h"
#include "ui_pagelight.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageLight::PageLight(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageLight)
{
    ui->setupUi(this);
}

PageLight::~PageLight()
{
    delete ui;
}

void PageLight::setData(const tinygltf::Model &model, int index)
{

}
