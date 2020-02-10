#include "pageanimation.h"
#include "mainwindow.h"
#include "ui_pageanimation.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageAnimation::PageAnimation(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageAnimation)
{
    ui->setupUi(this);
}

PageAnimation::~PageAnimation()
{
    delete ui;
}

void PageAnimation::setData(const tinygltf::Model &model, int index)
{

}
