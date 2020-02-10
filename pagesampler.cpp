#include "pagesampler.h"
#include "mainwindow.h"
#include "ui_pagesampler.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageSampler::PageSampler(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageSampler)
{
    ui->setupUi(this);
}

PageSampler::~PageSampler()
{
    delete ui;
}

void PageSampler::setData(const tinygltf::Model &model, int index, int subIndex)
{

}
