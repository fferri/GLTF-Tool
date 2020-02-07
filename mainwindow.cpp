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

int recordSize(const tinygltf::Accessor &a)
{
    return tinygltf::GetComponentSizeInBytes(a.componentType) * tinygltf::GetNumComponentsInType(a.type);
}

template<typename T>
void addRecord(QTableWidget *w, int row, const T *b, int n)
{
    w->setRowCount(row + 1);
    for(int col = 0; col < n; col++)
    {
        QTableWidgetItem *item = w->item(row, col);
        if(!item)
        {
            item = new QTableWidgetItem;
            w->setItem(row, col, item);
        }
        item->setText(QString::number(b[col]));
    }
}

void addRecord(QTableWidget *w, int row, const QByteArray &a, int offset, int compType, int type)
{
    int n = tinygltf::GetNumComponentsInType(type);
    switch(compType)
    {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
        return addRecord(w, row, reinterpret_cast<const char*>(a.data() + offset), n);
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        return addRecord(w, row, reinterpret_cast<const unsigned char*>(a.data() + offset), n);
    case TINYGLTF_COMPONENT_TYPE_SHORT:
        return addRecord(w, row, reinterpret_cast<const short*>(a.data() + offset), n);
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        return addRecord(w, row, reinterpret_cast<const unsigned short*>(a.data() + offset), n);
    case TINYGLTF_COMPONENT_TYPE_INT:
        return addRecord(w, row, reinterpret_cast<const int*>(a.data() + offset), n);
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        return addRecord(w, row, reinterpret_cast<const unsigned int*>(a.data() + offset), n);
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
        return addRecord(w, row, reinterpret_cast<const float*>(a.data() + offset), n);
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
        return addRecord(w, row, reinterpret_cast<const double*>(a.data() + offset), n);
    }
}

void setRecordHeaders(QTableWidget *w, int type)
{
    w->setColumnCount(tinygltf::GetNumComponentsInType(type));
    switch(type)
    {
    case TINYGLTF_TYPE_SCALAR:
        w->setHorizontalHeaderLabels(QStringList() << "value");
        break;
    case TINYGLTF_TYPE_VEC2:
        w->setHorizontalHeaderLabels(QStringList() << "x" << "y");
        break;
    case TINYGLTF_TYPE_VEC3:
        w->setHorizontalHeaderLabels(QStringList() << "x" << "y" << "z");
        break;
    case TINYGLTF_TYPE_VEC4:
        w->setHorizontalHeaderLabels(QStringList() << "x" << "y" << "z" << "w");
        break;
    case TINYGLTF_TYPE_MAT2:
        w->setHorizontalHeaderLabels(QStringList() << "m11" << "m12" << "m21" << "m22");
        break;
    case TINYGLTF_TYPE_MAT3:
        w->setHorizontalHeaderLabels(QStringList() << "m11" << "m12" << "m13" << "m21" << "m22" << "m23" << "m31" << "m32" << "m33");
        break;
    case TINYGLTF_TYPE_MAT4:
        w->setHorizontalHeaderLabels(QStringList() << "m11" << "m12" << "m13" << "m14" << "m21" << "m22" << "m23" << "m24" << "m31" << "m32" << "m33" << "m34" << "m41" << "m42" << "m43" << "m44");
        break;
    }
}

void selectWidget(Ui::MainWindow *ui, GLTFModel::Group g)
{
    switch(g)
    {
    case GLTFModel::Accessors:   return ui->stackedWidget->setCurrentWidget(ui->accessorWidget);
    case GLTFModel::Animations:  return ui->stackedWidget->setCurrentWidget(ui->animationWidget);
    case GLTFModel::Buffers:     return ui->stackedWidget->setCurrentWidget(ui->bufferWidget);
    case GLTFModel::BufferViews: return ui->stackedWidget->setCurrentWidget(ui->bufferViewWidget);
    case GLTFModel::Materials:   return ui->stackedWidget->setCurrentWidget(ui->materialWidget);
    case GLTFModel::Meshes:      return ui->stackedWidget->setCurrentWidget(ui->meshWidget);
    case GLTFModel::Nodes:       return ui->stackedWidget->setCurrentWidget(ui->nodeWidget);
    case GLTFModel::Textures:    return ui->stackedWidget->setCurrentWidget(ui->textureWidget);
    case GLTFModel::Images:      return ui->stackedWidget->setCurrentWidget(ui->imageWidget);
    case GLTFModel::Skins:       return ui->stackedWidget->setCurrentWidget(ui->skinWidget);
    case GLTFModel::Samplers:    return ui->stackedWidget->setCurrentWidget(ui->samplerWidget);
    case GLTFModel::Cameras:     return ui->stackedWidget->setCurrentWidget(ui->cameraWidget);
    case GLTFModel::Scenes:      return ui->stackedWidget->setCurrentWidget(ui->sceneWidget);
    case GLTFModel::Lights:      return ui->stackedWidget->setCurrentWidget(ui->lightWidget);
    }
}

QString accessorTypeString(int x)
{
    switch(x)
    {
    case TINYGLTF_TYPE_SCALAR: return "scalar";
    case TINYGLTF_TYPE_VEC2: return "vec2";
    case TINYGLTF_TYPE_VEC3: return "vec3";
    case TINYGLTF_TYPE_VEC4: return "vec4";
    case TINYGLTF_TYPE_MAT2: return "mat2";
    case TINYGLTF_TYPE_MAT3: return "mat3";
    case TINYGLTF_TYPE_MAT4: return "mat4";
    }
}

QString accessorComponentTypeString(int x)
{
    switch(x)
    {
    case TINYGLTF_COMPONENT_TYPE_BYTE: return "byte";
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: return "unsigned byte";
    case TINYGLTF_COMPONENT_TYPE_SHORT: return "short";
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: return "unsigned short";
    case TINYGLTF_COMPONENT_TYPE_INT: return "int";
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: return "unsigned int";
    case TINYGLTF_COMPONENT_TYPE_FLOAT: return "float";
    case TINYGLTF_COMPONENT_TYPE_DOUBLE: return "double";
    }
    return "?";
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

void MainWindow::selectItem(GLTFModel::Group group, int idx, bool syncTree)
{
    if(idx < 0)
    {
        ui->stackedWidget->setCurrentWidget(ui->defaultPage);
        return;
    }

    selectWidget(ui, group);

    if(syncTree)
    {
        QModelIndex treeIndex = model->encodeIndex(group, idx);
        ui->treeView->setCurrentIndex(treeIndex);
    }

    switch(group)
    {
    case GLTFModel::Buffers:
        {
            tinygltf::Buffer &buffer = gltf.buffers[idx];
            ui->bufferIndex->setText(QString::number(idx));
            ui->bufferName->setText(QString::fromStdString(buffer.name));
            ui->bufferData->setData(getBuffer(&gltf, idx));
        }
        break;
    case GLTFModel::BufferViews:
        {
            tinygltf::BufferView &bufferView = gltf.bufferViews[idx];
            ui->bufferViewIndex->setText(QString::number(idx));
            ui->bufferViewName->setText(QString::fromStdString(bufferView.name));
            ui->bufferViewBuffer->setText(bufferView.buffer == -1 ? "None" : QString("<a href='#Buffers=%1'>%1</a>").arg(bufferView.buffer));
            ui->bufferViewData->setData(getBufferView(&gltf, idx));
        }
        break;
    case GLTFModel::Accessors:
        {
            tinygltf::Accessor &accessor = gltf.accessors[idx];
            ui->accessorIndex->setText(QString::number(idx));
            ui->accessorName->setText(QString::fromStdString(accessor.name));
            ui->accessorType->setText(accessorTypeString(accessor.type));
            ui->accessorComponentType->setText(accessorComponentTypeString(accessor.componentType));
            ui->accessorBufferView->setText(QString("<a href='#BufferViews=%1'>%1</a>").arg(accessor.bufferView));
            QByteArray data = getBufferView(&gltf, accessor.bufferView);
            setRecordHeaders(ui->accessorData, accessor.type);
            for(int i = accessor.byteOffset, row = 0; i < data.size(); i += accessor.byteOffset + recordSize(accessor), row++)
                addRecord(ui->accessorData, row, data, i, accessor.componentType, accessor.type);
            QStringList h;
            for(int i = 0; i < ui->accessorData->rowCount(); i++)
                h << QString::number(i);
            ui->accessorData->setVerticalHeaderLabels(h);
        }
        break;
    case GLTFModel::Images:
        {
            tinygltf::Image &image = gltf.images[idx];
            ui->imageIndex->setText(QString::number(idx));
            ui->imageName->setText(QString::fromStdString(image.name));
            ui->imageBufferView->setText(image.bufferView == -1 ? "None" : QString("<a href='#BufferViews=%1'>%1</a>").arg(image.bufferView));
            QByteArray data = getBufferView(&gltf, image.bufferView);
            QImage img = QImage::fromData(data);
            if(img.isNull()) qDebug() << "image is null" << data;
            QPixmap pix = QPixmap::fromImage(img);
            ui->imagePreview->setPixmap(pix);
            ui->imageScrollAreaContents->resize(img.size());
        }
        break;
    case GLTFModel::Nodes:
        {
            tinygltf::Node &node = gltf.nodes[idx];
            ui->nodeIndex->setText(QString::number(idx));
            ui->nodeName->setText(QString::fromStdString(node.name));
            ui->nodeTranslation->setText(node.translation.empty() ? "" : vec2str(node.translation));
            ui->nodeRotation->setText(node.rotation.empty() ? "" : vec2str(node.rotation));
            ui->nodeScale->setText(node.scale.empty() ? "" : vec2str(node.scale));
            ui->nodeMatrix->setText(node.matrix.empty() ? "" : mat2str(node.matrix));
            ui->nodeMesh->setText(node.mesh == -1 ? "None" : QString("<a href='#Meshes=%1'>%1</a>").arg(node.mesh));
            ui->nodeCamera->setText(node.camera == -1 ? "None" : QString("<a href='#Cameras=%1'>%1</a>").arg(node.camera));
        }
        break;
    case GLTFModel::Meshes:
        {
            tinygltf::Mesh &mesh = gltf.meshes[idx];
            ui->meshIndex->setText(QString::number(idx));
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
                tinygltf::Primitive &primitive = mesh.primitives[subIdx];
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
                    /*
                    QTableWidgetItem *itemValue = ui->meshPrimitiveAttributes->item(row, 1);
                    if(!itemValue)
                    {
                        itemValue = new QTableWidgetItem;
                        ui->meshPrimitiveAttributes->setItem(row, 1, itemValue);
                    }
                    itemValue->setText(QString("<a href='#Accessors=%1'>%1</a>").arg(p.second));
                    */
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
        break;
    }
}

void MainWindow::selectItemByLink(const QString &link)
{
    QString l(link);
    if(l[0] == "#") l = l.mid(1);
    auto y = l.split("=");
    auto z = y[0];
    GLTFModel::Group group = GLTFModel::None;
    if(z == "") group = GLTFModel::None;
    else if(z == "Accessors") group = GLTFModel::Accessors;
    else if(z == "Animations") group = GLTFModel::Animations;
    else if(z == "Buffers") group = GLTFModel::Buffers;
    else if(z == "BufferViews") group = GLTFModel::BufferViews;
    else if(z == "Materials") group = GLTFModel::Materials;
    else if(z == "Meshes") group = GLTFModel::Meshes;
    else if(z == "Nodes") group = GLTFModel::Nodes;
    else if(z == "Textures") group = GLTFModel::Textures;
    else if(z == "Images") group = GLTFModel::Images;
    else if(z == "Skins") group = GLTFModel::Skins;
    else if(z == "Samplers") group = GLTFModel::Samplers;
    else if(z == "Cameras") group = GLTFModel::Cameras;
    else if(z == "Scenes") group = GLTFModel::Scenes;
    else if(z == "Lights") group = GLTFModel::Lights;
    selectItem(group, y[1].toInt());
}

