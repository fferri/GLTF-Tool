#include "pagebuffer.h"
#include "mainwindow.h"
#include "ui_pagebuffer.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageBuffer::PageBuffer(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageBuffer)
{
    ui->setupUi(this);
    ui->data->setReadOnly(true);
}

PageBuffer::~PageBuffer()
{
    delete ui;
}

void PageBuffer::setData(const tinygltf::Model &model, int index)
{
    const tinygltf::Buffer &buffer = model.buffers[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(buffer.name));
    ui->data->setData(GLTFInfo::getBuffer(&model, index));
}
