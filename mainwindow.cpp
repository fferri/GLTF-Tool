#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gltfinfo.h"

tinygltf::Model gltf;
tinygltf::TinyGLTF loader;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      showFunc({
          {GLTFModel::Accessors, &MainWindow::showAccessor},
          {GLTFModel::Animations, &MainWindow::showAnimation},
          {GLTFModel::Buffers, &MainWindow::showBuffer},
          {GLTFModel::BufferViews, &MainWindow::showBufferView},
          {GLTFModel::Materials, &MainWindow::showMaterial},
          {GLTFModel::Meshes, &MainWindow::showMesh},
          {GLTFModel::Nodes, &MainWindow::showNode},
          {GLTFModel::Textures, &MainWindow::showTexture},
          {GLTFModel::Images, &MainWindow::showImage},
          {GLTFModel::Skins, &MainWindow::showSkin},
          {GLTFModel::Samplers, &MainWindow::showSampler},
          {GLTFModel::Cameras, &MainWindow::showCamera},
          {GLTFModel::Scenes, &MainWindow::showScene},
          {GLTFModel::Lights, &MainWindow::showLight},
      })
{
    ui->setupUi(this);

    loader.LoadASCIIFromFile(&gltf, nullptr, nullptr, "/Users/me/Desktop/youBotAndHanoiTower.gltf");

    model = new GLTFModel(this);
    model->setGLTF(&gltf);
    ui->treeView->setModel(model);

    ui->accessorData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->bufferData->setReadOnly(true);
    ui->bufferViewData->setReadOnly(true);
    ui->meshPrimitiveAttributes->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onItemSelected);
    connect(ui->nodeMesh, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
    connect(ui->nodeCamera, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
    connect(ui->accessorBufferView, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
    connect(ui->imageBufferView, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
    connect(ui->bufferViewBuffer, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
    connect(ui->meshPrimitiveMaterial, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
    connect(ui->meshPrimitiveIndices, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray getBuffer(const tinygltf::Model *model, int i)
{
    const tinygltf::Buffer &buffer = model->buffers[i];
    return QByteArray(reinterpret_cast<const char*>(buffer.data.data()), buffer.data.size());
}

QByteArray getBufferView(const tinygltf::Model *model, int i)
{
    const tinygltf::BufferView &bufferView = model->bufferViews[i];
    auto buffer = getBuffer(model, bufferView.buffer);
    QByteArray data;
    for(int i = bufferView.byteOffset; i < bufferView.byteOffset + bufferView.byteLength; i++)
        data.append(buffer.at(i));
    return data;
}

void addRecord(QTableWidget *w, int row, const QByteArray &a, int offset, int compType, int type)
{
    int n = tinygltf::GetNumComponentsInType(type);
    auto cells = GLTFInfo::cells[compType](a.data() + offset, n);
    w->setRowCount(row + 1);
    for(int col = 0; col < n; col++)
    {
        QTableWidgetItem *item = w->item(row, col);
        if(!item)
        {
            item = new QTableWidgetItem;
            w->setItem(row, col, item);
        }
        item->setText(cells[col]);
    }
}

template<typename T>
QString vec2str(const std::vector<T> &v)
{
    QStringList s;
    for(auto x : v) s << QString::number(x);
    return s.join(" ");
}

template<typename T>
QString mat2str(const std::vector<T> &v, int cols = 4)
{
    QStringList s;
    for(int i = 0; i < v.size(); i += cols)
    {
        QStringList t;
        for(int j = 0; j < cols; j++)
            t << QString::number(v[i + j]);
        s << t.join(" ");
    }
    return s.join("\n");
}

void MainWindow::onItemSelected(const QModelIndex &index, const QModelIndex &prev)
{
    auto p = model->decodeIndex(index);
    selectItem(p.first, p.second, false);
}

void MainWindow::selectItem(GLTFModel::Group group, int index, bool syncTree)
{
    if(index < 0 || group == GLTFModel::None)
    {
        ui->stackedWidget->setCurrentWidget(ui->defaultPage);
        return;
    }

    if(syncTree)
    {
        QModelIndex treeIndex = model->encodeIndex(group, index);
        ui->treeView->setCurrentIndex(treeIndex);
    }

    (this->*(showFunc[group]))(index);
}

void MainWindow::selectItemByLink(const QString &link)
{
    QString l(link);
    if(l[0] == "#") l = l.mid(1);
    auto y = l.split("=");
    auto z = y[0];
    QMetaEnum groupMeta = QMetaEnum::fromType<GLTFModel::Group>();
    auto group = static_cast<GLTFModel::Group>(groupMeta.keyToValue(link.toUtf8().data()));
    selectItem(group, y[1].toInt());
}

void MainWindow::showAccessor(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->accessorWidget);
    const tinygltf::Accessor &accessor = gltf.accessors[index];
    ui->accessorIndex->setText(QString::number(index));
    ui->accessorName->setText(QString::fromStdString(accessor.name));
    ui->accessorType->setText(GLTFInfo::typeStr[accessor.type]);
    ui->accessorComponentType->setText(GLTFInfo::compTypeStr[accessor.componentType]);
    ui->accessorBufferView->setText(QString("<a href='#BufferViews=%1'>%1</a>").arg(accessor.bufferView));
    QByteArray data = getBufferView(&gltf, accessor.bufferView);
    ui->accessorData->setColumnCount(tinygltf::GetNumComponentsInType(accessor.type));
    ui->accessorData->setHorizontalHeaderLabels(GLTFInfo::typeFields[accessor.type]);
    int recordSize = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);
    for(int i = accessor.byteOffset, row = 0; i < data.size(); i += accessor.byteOffset + recordSize, row++)
        addRecord(ui->accessorData, row, data, i, accessor.componentType, accessor.type);
    QStringList h;
    for(int i = 0; i < ui->accessorData->rowCount(); i++)
        h << QString::number(i);
    ui->accessorData->setVerticalHeaderLabels(h);
}

void MainWindow::showAnimation(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->animationWidget);
    const tinygltf::Animation &animation = gltf.animations[index];
}

void MainWindow::showBuffer(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->bufferWidget);
    const tinygltf::Buffer &buffer = gltf.buffers[index];
    ui->bufferIndex->setText(QString::number(index));
    ui->bufferName->setText(QString::fromStdString(buffer.name));
    ui->bufferData->setData(getBuffer(&gltf, index));

}

void MainWindow::showBufferView(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->bufferViewWidget);
    const tinygltf::BufferView &bufferView = gltf.bufferViews[index];
    ui->bufferViewIndex->setText(QString::number(index));
    ui->bufferViewName->setText(QString::fromStdString(bufferView.name));
    ui->bufferViewBuffer->setText(bufferView.buffer == -1 ? "None" : QString("<a href='#Buffers=%1'>%1</a>").arg(bufferView.buffer));
    ui->bufferViewData->setData(getBufferView(&gltf, index));
}

void MainWindow::showMaterial(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->materialWidget);
    const tinygltf::Material &material = gltf.materials[index];
}

void MainWindow::showMesh(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->meshWidget);
    const tinygltf::Mesh &mesh = gltf.meshes[index];
    ui->meshIndex->setText(QString::number(index));
    ui->meshName->setText(QString::fromStdString(mesh.name));
    if(mesh.primitives.empty())
    {
        ui->meshStackedWidget->setCurrentWidget(ui->meshPageOverview);
        ui->meshPrimitives->setText(QString::number(mesh.primitives.size()));
    }
    else
    {
        ui->meshStackedWidget->setCurrentWidget(ui->meshPagePrimitive);
        int subIdx = 0;
        const tinygltf::Primitive &primitive = mesh.primitives[subIdx];
        ui->meshPrimitiveIndex->setText(QString::number(subIdx));
        ui->meshPrimitiveMaterial->setText(primitive.material == -1 ? "None" : QString("<a href='#Materials=%1'>%1</a>").arg(primitive.material));
        ui->meshPrimitiveIndices->setText(primitive.indices == -1 ? "None" : QString("<a href='#Accessors=%1'>%1</a>").arg(primitive.indices));
        ui->meshPrimitiveMode->setText(QString::number(primitive.mode));
        ui->meshPrimitiveAttributes->setRowCount(primitive.attributes.size());
        ui->meshPrimitiveAttributes->setColumnCount(2);
        int row = 0;
        for(const auto &p : primitive.attributes)
        {
            QTableWidgetItem *itemKey = ui->meshPrimitiveAttributes->item(row, 0);
            if(!itemKey)
            {
                itemKey = new QTableWidgetItem;
                ui->meshPrimitiveAttributes->setItem(row, 0, itemKey);
            }
            itemKey->setText(QString::fromStdString(p.first));
            QLabel *label = new QLabel(QString("<a href='#Accessors=%1'>%1</a>").arg(p.second));
            connect(label, &QLabel::linkActivated, this, &MainWindow::selectItemByLink);
            ui->meshPrimitiveAttributes->setCellWidget(row, 1, label);
            QPalette palette = ui->meshPrimitiveAttributes->palette();
            palette.setBrush(QPalette::Highlight, palette.brush(QPalette::Base));
            palette.setBrush(QPalette::HighlightedText, palette.brush(QPalette::Text));
            ui->meshPrimitiveAttributes->setPalette(palette);
            row++;
        }
    }
}

void MainWindow::showNode(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->nodeWidget);
    const tinygltf::Node &node = gltf.nodes[index];
    ui->nodeIndex->setText(QString::number(index));
    ui->nodeName->setText(QString::fromStdString(node.name));
    ui->nodeTranslation->setText(node.translation.empty() ? "" : vec2str(node.translation));
    ui->nodeRotation->setText(node.rotation.empty() ? "" : vec2str(node.rotation));
    ui->nodeScale->setText(node.scale.empty() ? "" : vec2str(node.scale));
    ui->nodeMatrix->setText(node.matrix.empty() ? "" : mat2str(node.matrix));
    ui->nodeMesh->setText(node.mesh == -1 ? "None" : QString("<a href='#Meshes=%1'>%1</a>").arg(node.mesh));
    ui->nodeCamera->setText(node.camera == -1 ? "None" : QString("<a href='#Cameras=%1'>%1</a>").arg(node.camera));
}

void MainWindow::showTexture(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->textureWidget);
    const tinygltf::Texture &texture = gltf.textures[index];
}

void MainWindow::showImage(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->imageWidget);
    const tinygltf::Image &image = gltf.images[index];
    ui->imageIndex->setText(QString::number(index));
    ui->imageName->setText(QString::fromStdString(image.name));
    ui->imageBufferView->setText(image.bufferView == -1 ? "None" : QString("<a href='#BufferViews=%1'>%1</a>").arg(image.bufferView));
    QByteArray data = getBufferView(&gltf, image.bufferView);
    QImage img = QImage::fromData(data);
    if(img.isNull()) qDebug() << "image is null" << data;
    QPixmap pix = QPixmap::fromImage(img);
    ui->imagePreview->setPixmap(pix);
    ui->imageScrollAreaContents->resize(img.size());
}

void MainWindow::showSkin(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->skinWidget);
    const tinygltf::Skin &skin = gltf.skins[index];
}

void MainWindow::showSampler(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->samplerWidget);
    const tinygltf::Sampler &sampler = gltf.samplers[index];
}

void MainWindow::showCamera(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->cameraWidget);
    const tinygltf::Camera &camera = gltf.cameras[index];
}

void MainWindow::showScene(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->sceneWidget);
    const tinygltf::Scene &scene = gltf.scenes[index];
}

void MainWindow::showLight(int index)
{
    ui->stackedWidget->setCurrentWidget(ui->lightWidget);
    const tinygltf::Light &light = gltf.lights[index];
}
