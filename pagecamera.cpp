#include "pagecamera.h"
#include "mainwindow.h"
#include "ui_pagecamera.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageCamera::PageCamera(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageCamera)
{
    ui->setupUi(this);
}

PageCamera::~PageCamera()
{
    delete ui;
}

void PageCamera::setData(const tinygltf::Model &model, int index)
{

}
