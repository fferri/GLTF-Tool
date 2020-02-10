#include "pageaccessor.h"
#include "mainwindow.h"
#include "ui_pageaccessor.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

PageAccessor::PageAccessor(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageAccessor)
{
    ui->setupUi(this);
    ui->data->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->bufferView, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
}

PageAccessor::~PageAccessor()
{
    delete ui;
}

void PageAccessor::setData(const tinygltf::Model &model, int index)
{
    const tinygltf::Accessor &accessor = model.accessors[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(accessor.name));
    ui->type->setText(GLTFInfo::typeStr[accessor.type]);
    ui->componentType->setText(GLTFInfo::compTypeStr[accessor.componentType]);
    ui->bufferView->setText(mainWindow()->link(GLTFModel::BufferViews, accessor.bufferView));
    QByteArray data = GLTFInfo::getBufferView(&model, accessor.bufferView);
    ui->data->setColumnCount(tinygltf::GetNumComponentsInType(accessor.type));
    ui->data->setHorizontalHeaderLabels(GLTFInfo::typeFields[accessor.type]);
    int recordSize = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);
    for(int i = accessor.byteOffset, row = 0; i < data.size(); i += accessor.byteOffset + recordSize, row++)
    {
        int n = tinygltf::GetNumComponentsInType(accessor.type);
        auto cells = GLTFInfo::cells[accessor.componentType](data.data() + i, n);
        ui->data->setRowCount(row + 1);
        for(int col = 0; col < n; col++)
        {
            QTableWidgetItem *item = ui->data->item(row, col);
            if(!item)
            {
                item = new QTableWidgetItem;
                ui->data->setItem(row, col, item);
            }
            item->setText(cells[col]);
        }
    }
    QStringList h;
    for(int i = 0; i < ui->data->rowCount(); i++)
        h << QString::number(i);
    ui->data->setVerticalHeaderLabels(h);
}
