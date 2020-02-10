#include "mainwindow.h"
#include "pageaccessor.h"
#include "pageanimation.h"
#include "pagebuffer.h"
#include "pagebufferview.h"
#include "pagecamera.h"
#include "pageimage.h"
#include "pagelight.h"
#include "pagematerial.h"
#include "pagemesh.h"
#include "pagenode.h"
#include "pagesampler.h"
#include "pagescene.h"
#include "pageskin.h"
#include "pagetexture.h"
#include "ui_mainwindow.h"
#include "gltfinfo.h"

tinygltf::Model gltf;
tinygltf::TinyGLTF loader;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , page({
                {GLTFModel::Accessors, new PageAccessor(this)},
                {GLTFModel::Animations, new PageAnimation(this)},
                {GLTFModel::Buffers, new PageBuffer(this)},
                {GLTFModel::BufferViews, new PageBufferView(this)},
                {GLTFModel::Materials, new PageMaterial(this)},
                {GLTFModel::Meshes, new PageMesh(this)},
                {GLTFModel::Nodes, new PageNode(this)},
                {GLTFModel::Textures, new PageTexture(this)},
                {GLTFModel::Images, new PageImage(this)},
                {GLTFModel::Skins, new PageSkin(this)},
                {GLTFModel::Samplers, new PageSampler(this)},
                {GLTFModel::Cameras, new PageCamera(this)},
                {GLTFModel::Scenes, new PageScene(this)},
                {GLTFModel::Lights, new PageLight(this)},
           })
{
    ui->setupUi(this);    
    for(auto p : page.values())
        ui->stackedWidget->addWidget(p);

    loader.LoadASCIIFromFile(&gltf, nullptr, nullptr, "/Users/me/Desktop/youBotAndHanoiTower.gltf");

    model_ = new GLTFModel(this);
    model_->setGLTF(&gltf);
    ui->treeView->setModel(model_);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onItemSelected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::decodeLink(const QString &link, GLTFModel::Group &group, int &index)
{
    QRegularExpression re("^#(\\w+)=(\\d+)$");
    QRegularExpressionMatch match = re.match(link);
    if(!match.hasMatch()) return false;
    QMetaEnum groupMeta = QMetaEnum::fromType<GLTFModel::Group>();
    group = static_cast<GLTFModel::Group>(groupMeta.keyToValue(match.captured(1).toUtf8().data()));
    index = match.captured(2).toInt();
    return true;
}

QString MainWindow::link(GLTFModel::Group group, int index)
{
    if(index < 0) return QStringLiteral("None");
    QMetaEnum groupMeta = QMetaEnum::fromType<GLTFModel::Group>();
    return QString("<a href='#%1=%2'>%2</a>").arg(groupMeta.valueToKey(group)).arg(index);
}

void MainWindow::onItemSelected(const QModelIndex &index, const QModelIndex &prev)
{
    auto p = model_->decodeIndex(index);
    selectItem(p.first, p.second, false);
}

void MainWindow::selectItem(GLTFModel::Group group, int index, int subIndex, bool syncTree)
{
    auto w = page.value(group);
    if(w && index >= 0)
    {
        if(syncTree)
        {
            QModelIndex treeIndex = model_->encodeIndex(group, index);
            ui->treeView->setCurrentIndex(treeIndex);
        }
        w->setData(gltf, index, subIndex);
        ui->stackedWidget->setCurrentWidget(w);
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->defaultPage);
    }
}

void MainWindow::selectItemByLink(const QString &link)
{
    GLTFModel::Group group;
    int index;
    if(decodeLink(link, group, index))
        selectItem(group, index);
}
