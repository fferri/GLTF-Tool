#include "pageimage.h"
#include "mainwindow.h"
#include "ui_pageimage.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

#include <cstring>

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

void PageImage::setData(const tinygltf::Model &model, int index)
{
    if(index < 0 || index >= (int)model.images.size()) return;

    const tinygltf::Image &image = model.images[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(image.name));
    ui->bufferView->setText(mainWindow()->link(GLTFModel::BufferViews, image.bufferView));

    QImage img;
    if(image.bufferView >= 0)
    {
        // Image data stored in a buffer view is kept encoded in the glTF.
        img = QImage::fromData(GLTFInfo::getBufferView(&model, image.bufferView));
    }
    else if(image.width > 0 && image.height > 0 && image.component == 4)
    {
        // Images loaded from a URI are already decoded by tinygltf.
        if(image.bits == 8)
        {
            img = QImage(image.width, image.height, QImage::Format_RGBA8888);
            for(int y = 0; y < image.height; y++)
                std::memcpy(img.scanLine(y), image.image.data() + (size_t)y * image.width * 4, (size_t)image.width * 4);
        }
        else if(image.bits == 16)
        {
            img = QImage(image.width, image.height, QImage::Format_ARGB32);
            for(int y = 0; y < image.height; y++)
            {
                QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
                const quint16 *src = reinterpret_cast<const quint16*>(image.image.data() + (size_t)y * image.width * 8);
                for(int x = 0; x < image.width; x++)
                    line[x] = qRgba(src[x * 4] >> 8, src[x * 4 + 1] >> 8,
                                    src[x * 4 + 2] >> 8, src[x * 4 + 3] >> 8);
            }
        }
    }

    if(img.isNull())
    {
        ui->preview->setText("Unable to display image");
        return;
    }

    ui->preview->setPixmap(QPixmap::fromImage(img));
    ui->scrollAreaContents->resize(img.size());
}
