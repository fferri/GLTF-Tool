#include "pagescene.h"
#include "mainwindow.h"
#include "ui_pagescene.h"
#include "tiny_gltf.h"
#include "gltfinfo.h"

#include <QHeaderView>

namespace
{
QLabel *makeLinkLabel(QWidget *parent, MainWindow *mainWindow, GLTFModel::Group group, int index)
{
    QLabel *label = new QLabel(parent);
    label->setText(mainWindow->link(group, index));
    label->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    QObject::connect(label, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
    return label;
}
}

PageScene::PageScene(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageScene)
{
    ui->setupUi(this);

    ui->nodes->setColumnCount(3);
    ui->nodes->setSelectionMode(QAbstractItemView::NoSelection);
    ui->nodes->setHorizontalHeaderLabels(QStringList() << "Row" << "Node" << "Name");
    ui->nodes->verticalHeader()->setVisible(false);
    ui->nodes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->nodes->horizontalHeader()->setStretchLastSection(true);
}

PageScene::~PageScene()
{
    delete ui;
}

void PageScene::setData(const tinygltf::Model &model, int index)
{
    if(index < 0 || index >= (int)model.scenes.size()) return;

    const tinygltf::Scene &scene = model.scenes[index];

    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(scene.name));
    ui->defaultScene->setText(model.defaultScene == index ? "Yes" : "No");
    ui->nodesCount->setText(QString::number(scene.nodes.size()));

    QStringList extensions;
    for(const auto &extension : scene.extensions)
        extensions << QString::fromStdString(extension.first);
    extensions.sort();
    ui->extensions->setText(extensions.isEmpty() ? "None" : extensions.join(", "));

    QTableWidget *table = ui->nodes;
    table->setRowCount(0);
    table->setRowCount(scene.nodes.size());

    for(int row = 0; row < (int)scene.nodes.size(); row++)
    {
        int node = scene.nodes[row];

        table->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
        if(node >= 0 && node < (int)model.nodes.size())
        {
            table->setCellWidget(row, 1, makeLinkLabel(table, mainWindow(), GLTFModel::Nodes, node));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(model.nodes[node].name)));
        }
        else
        {
            table->setItem(row, 1, new QTableWidgetItem(QString("invalid (%1)").arg(node)));
            table->setItem(row, 2, new QTableWidgetItem);
        }
    }
    table->clearSelection();
    table->setCurrentCell(-1, -1);
}
