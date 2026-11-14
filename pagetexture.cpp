#include "pagetexture.h"
#include "mainwindow.h"
#include "ui_pagetexture.h"
#include "tiny_gltf.h"
#include "gltfinfo.h"

PageTexture::PageTexture(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageTexture)
{
    ui->setupUi(this);
    connect(ui->sampler, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
    connect(ui->source, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
}

PageTexture::~PageTexture()
{
    delete ui;
}

void PageTexture::setData(const tinygltf::Model &model, int index)
{
    if(index < 0 || index >= (int)model.textures.size()) return;

    const tinygltf::Texture &texture = model.textures[index];

    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(texture.name));

    if(texture.sampler >= 0)
        ui->sampler->setText(mainWindow()->link(GLTFModel::Samplers, texture.sampler));
    else
        ui->sampler->setText("Default (no sampler)");

    ui->source->setText(mainWindow()->link(GLTFModel::Images, texture.source));

    QStringList extensions;
    for(const auto &extension : texture.extensions)
        extensions << QString::fromStdString(extension.first);
    extensions.sort();
    ui->extensions->setText(extensions.isEmpty() ? "None" : extensions.join(", "));
}
