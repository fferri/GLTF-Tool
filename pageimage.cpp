#include "pageimage.h"
#include "mainwindow.h"
#include "ui_pageimage.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageImage::PageImage(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageImage)
{
    ui->setupUi(this);
    connect(ui->bufferView, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
}

PageImage::~PageImage()
{
    delete ui;
}

void PageImage::setData(const tinygltf::Model &model, int index, int subIndex)
{
    const tinygltf::Image &image = model.images[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(image.name));
    ui->bufferView->setText(mainWindow()->link(GLTFModel::BufferViews, image.bufferView));
    QByteArray data = GLTFInfo::getBufferView(&model, image.bufferView);
    QImage img = QImage::fromData(data);
    if(img.isNull()) qDebug() << "image is null" << data;
    QPixmap pix = QPixmap::fromImage(img);
    ui->preview->setPixmap(pix);
    ui->scrollAreaContents->resize(img.size());
}
