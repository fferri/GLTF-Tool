#include "pagenode.h"
#include "mainwindow.h"
#include "ui_pagenode.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageNode::PageNode(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageNode)
{
    ui->setupUi(this);
    connect(ui->mesh, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
    connect(ui->camera, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
}

PageNode::~PageNode()
{
    delete ui;
}

void PageNode::setData(const tinygltf::Model &model, int index)
{
    const tinygltf::Node &node = model.nodes[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(node.name));
    ui->translation->setText(node.translation.empty() ? "" : GLTFInfo::vec2str(node.translation));
    ui->rotation->setText(node.rotation.empty() ? "" : GLTFInfo::vec2str(node.rotation));
    ui->scale->setText(node.scale.empty() ? "" : GLTFInfo::vec2str(node.scale));
    ui->matrix->setText(node.matrix.empty() ? "" : GLTFInfo::mat2str(node.matrix));
    ui->mesh->setText(mainWindow()->link(GLTFModel::Meshes, node.mesh));
    ui->camera->setText(mainWindow()->link(GLTFModel::Cameras, node.camera));
}
