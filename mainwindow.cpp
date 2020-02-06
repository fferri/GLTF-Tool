#include "mainwindow.h"
#include "ui_mainwindow.h"

tinygltf::Model gltf;
tinygltf::TinyGLTF loader;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loader.LoadASCIIFromFile(&gltf, nullptr, nullptr, "/Users/me/Desktop/youBotAndHanoiTower.gltf");

    model = new GLTFModel(this);
    model->setGLTF(&gltf);
    ui->treeView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

