#include "pagemesh.h"
#include "mainwindow.h"
#include "ui_pagemesh.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageMesh::PageMesh(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageMesh)
{
    ui->setupUi(this);
    ui->primitiveAttributes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->primitiveAttributes->setHorizontalHeaderLabels(QStringList() << "Name" << "Value");
    connect(ui->primitiveMaterial, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
    connect(ui->primitiveIndices, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
}

PageMesh::~PageMesh()
{
    delete ui;
}

void PageMesh::setData(const tinygltf::Model &model, int index)
{
    model_ = &model;
    index_ = index;

    const tinygltf::Mesh &mesh = model.meshes[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(mesh.name));
    ui->primitives->setText(QString::number(mesh.primitives.size()));
    ui->primitive->clear();
    ui->primitiveWidget->setVisible(!mesh.primitives.empty());
    if(!mesh.primitives.empty())
    {
        for(int i = 0; i < mesh.primitives.size(); i++)
            ui->primitive->addItem(QString::number(i));
        ui->primitive->setCurrentIndex(0);
    }
}

void PageMesh::on_primitive_currentIndexChanged(int index)
{
    ui->primitiveWidget->setVisible(index >= 0);
    if(index == -1) return;
    const tinygltf::Mesh &mesh = model_->meshes[index_];
    const tinygltf::Primitive &primitive = mesh.primitives[index];
    ui->primitiveIndex->setText(QString::number(index));
    ui->primitiveMaterial->setText(mainWindow()->link(GLTFModel::Materials, primitive.material));
    ui->primitiveIndices->setText(mainWindow()->link(GLTFModel::Accessors, primitive.indices));
    ui->primitiveMode->setText(QString::number(primitive.mode));
    ui->primitiveAttributes->setRowCount(primitive.attributes.size());
    ui->primitiveAttributes->setColumnCount(2);
    int row = 0;
    for(const auto &p : primitive.attributes)
    {
        QTableWidgetItem *itemKey = ui->primitiveAttributes->item(row, 0);
        if(!itemKey)
        {
            itemKey = new QTableWidgetItem;
            ui->primitiveAttributes->setItem(row, 0, itemKey);
        }
        itemKey->setText(QString::fromStdString(p.first));
        QLabel *label = new QLabel(mainWindow()->link(GLTFModel::Accessors, p.second));
        connect(label, &QLabel::linkActivated, mainWindow(), &MainWindow::selectItemByLink);
        ui->primitiveAttributes->setCellWidget(row, 1, label);
        QPalette palette = ui->primitiveAttributes->palette();
        palette.setBrush(QPalette::Highlight, palette.brush(QPalette::Base));
        palette.setBrush(QPalette::HighlightedText, palette.brush(QPalette::Text));
        ui->primitiveAttributes->setPalette(palette);
        row++;
    }
}
