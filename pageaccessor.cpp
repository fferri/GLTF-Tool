#include "pageaccessor.h"
#include "mainwindow.h"
#include "ui_pageaccessor.h"
#include "tiny_gltf.h"
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
    if(index < 0 || index >= (int)model.accessors.size()) return;

    const tinygltf::Accessor &accessor = model.accessors[index];
    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(accessor.name));
    ui->type->setText(GLTFInfo::typeStr[accessor.type]);
    ui->componentType->setText(GLTFInfo::compTypeStr[accessor.componentType]);
    ui->bufferView->setText(mainWindow()->link(GLTFModel::BufferViews, accessor.bufferView));

    const int columns = GLTFInfo::componentCount(accessor.type);
    QVector<QStringList> rows = GLTFInfo::accessorData(&model, index);

    ui->data->setRowCount(0);
    ui->data->clearContents();
    ui->data->setColumnCount(columns);
    ui->data->setHorizontalHeaderLabels(GLTFInfo::typeFields.value(accessor.type));
    ui->data->setRowCount(rows.size());

    for(int row = 0; row < rows.size(); row++)
    {
        const QStringList &values = rows[row];
        for(int col = 0; col < values.size(); col++)
            ui->data->setItem(row, col, new QTableWidgetItem(values[col]));
    }

    QStringList h;
    h.reserve(rows.size());
    for(int i = 0; i < rows.size(); i++)
        h << QString::number(i);
    ui->data->setVerticalHeaderLabels(h);
}
