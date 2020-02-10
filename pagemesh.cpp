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

void PageMesh::setData(const tinygltf::Model &model, int index, int subIndex)
{
    const tinygltf::Mesh &mesh = model.meshes[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(mesh.name));
    if(mesh.primitives.empty() || subIndex < 0)
    {
        ui->stackedWidget->setCurrentWidget(ui->pageOverview);
        ui->primitives->setText(QString::number(mesh.primitives.size()));
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->pagePrimitive);
        const tinygltf::Primitive &primitive = mesh.primitives[subIndex];
        ui->primitiveIndex->setText(QString::number(subIndex));
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
}
