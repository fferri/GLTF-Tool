#include "pagescene.h"
#include "mainwindow.h"
#include "ui_pagescene.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageScene::PageScene(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageScene)
{
    ui->setupUi(this);
}

PageScene::~PageScene()
{
    delete ui;
}

void PageScene::setData(const tinygltf::Model &model, int index, int subIndex)
{

}
