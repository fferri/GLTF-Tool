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

    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onItemSelected);
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

void MainWindow::onItemSelected(const QModelIndex &index, const QModelIndex &prev)
{
    auto path = model->indexPath(index);
    if(path.size() == 2)
    {
        selectWidget(ui, static_cast<GLTFModel::Group>(path[0]));
        int i = path[1];
        switch(path[0])
        {
        case GLTFModel::Buffers:
            {
                tinygltf::Buffer &buffer = gltf.buffers[path[1]];
                ui->bufferLabel->setText(QString("Buffer %1 (%2)").arg(i).arg(QString::fromStdString(buffer.name)));
                ui->bufferData->setData(getBuffer(&gltf, path[1]));
            }
            break;
        case GLTFModel::BufferViews:
            {
                tinygltf::BufferView &bufferView = gltf.bufferViews[path[1]];
                ui->bufferViewLabel->setText(QString("Buffer View %1 (%2)").arg(i).arg(QString::fromStdString(bufferView.name)));
                ui->bufferData->setData(getBufferView(&gltf, path[1]));
            }
            break;
        case GLTFModel::Accessors:
            {
                tinygltf::Accessor &accessor = gltf.accessors[path[1]];
                ui->accessorLabel->setText(QString("Accessor %1 (%2)\nType: %3\nComponent type: %4").arg(i).arg(QString::fromStdString(accessor.name), accessorTypeString(accessor.type), accessorComponentTypeString(accessor.componentType)));
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
        }
        return;
    }
    ui->stackedWidget->setCurrentWidget(ui->defaultPage);
}
