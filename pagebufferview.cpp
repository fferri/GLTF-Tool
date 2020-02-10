#include "pagebufferview.h"
#include "mainwindow.h"
#include "ui_pagebufferview.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageBufferView::PageBufferView(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageBufferView)
{
    ui->setupUi(this);
    ui->data->setReadOnly(true);
    connect(ui->buffer, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
}

PageBufferView::~PageBufferView()
{
    delete ui;
}

void PageBufferView::setData(const tinygltf::Model &model, int index)
{
    const tinygltf::BufferView &bufferView = model.bufferViews[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(bufferView.name));
    ui->buffer->setText(mainWindow()->link(GLTFModel::Buffers, bufferView.buffer));
    ui->data->setData(GLTFInfo::getBufferView(&model, index));
}
