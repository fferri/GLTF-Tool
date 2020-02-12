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
    const tinygltf::Camera &camera = model.cameras[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(camera.name));
    ui->type->setText(QString::fromStdString(camera.type));
    if(camera.type == "perspective")
    {
        ui->stackedWidget->setCurrentWidget(ui->perspective);
        ui->aspectRatio->setText(QString::number(camera.perspective.aspectRatio));
        ui->yfov->setText(QString::number(camera.perspective.yfov));
        ui->znear->setText(QString::number(camera.perspective.znear));
        ui->zfar->setText(QString::number(camera.perspective.zfar));
    }
    else if(camera.type == "orthographic")
    {
        ui->stackedWidget->setCurrentWidget(ui->orthographic);
        ui->xmag->setText(QString::number(camera.orthographic.xmag));
        ui->ymag->setText(QString::number(camera.orthographic.ymag));
        ui->znearortho->setText(QString::number(camera.orthographic.znear));
        ui->zfarortho->setText(QString::number(camera.orthographic.zfar));
    }
    else ui->stackedWidget->setCurrentWidget(ui->unsupported);
}
