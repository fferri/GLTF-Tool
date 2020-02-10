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

#include <QFileDialog>

tinygltf::Model gltf;
tinygltf::TinyGLTF loader;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model_(nullptr)
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
    ui->actionBack->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    ui->actionForward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    for(auto p : page.values())
        ui->stackedWidget->addWidget(p);
    history.mainWindow = this;
    history.clear();

    QFile f("/Users/me/Desktop/youBotAndHanoiTower.gltf");
    if(f.exists()) openFile(f.fileName());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QString &filePath)
{
    if(filePath.endsWith(".gltf"))
        loader.LoadASCIIFromFile(&gltf, nullptr, nullptr, filePath.toStdString());
    else if(filePath.endsWith(".glb"))
        loader.LoadBinaryFromFile(&gltf, nullptr, nullptr, filePath.toStdString());
    else return;

    if(model_)
    {
        model_->deleteLater();
        model_ = nullptr;
    }

    history.clear();
    model_= new GLTFModel(this);
    model_->setGLTF(&gltf);
    ui->treeView->setModel(model_);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onItemSelected);
    selectItem(GLTFModel::None, -1, -1);
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

void MainWindow::onItemSelected(const QModelIndex &modelIndex, const QModelIndex &prevModelIndex)
{
    auto p = model_->decodeIndex(modelIndex);
    GLTFModel::Group group = p.first;
    int index = p.second;

    history.push(modelIndex);
    auto w = page.value(group);
    if(w && index >= 0)
    {
        w->setData(gltf, index, -1);
        ui->stackedWidget->setCurrentWidget(w);
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->defaultPage);
    }
}

void MainWindow::selectItem(GLTFModel::Group group, int index, int subIndex)
{
    ui->treeView->setCurrentIndex(model_->encodeIndex(group, index));
}

void MainWindow::selectItemByLink(const QString &link)
{
    GLTFModel::Group group;
    int index;
    if(decodeLink(link, group, index))
    {
        selectItem(group, index, -1);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("GLTF files (*.gltf *.glb)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if(dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        openFile(fileNames[0]);
    }
}

void MainWindow::on_actionBack_triggered()
{
    if(history.back())
    {
        auto p = model_->decodeIndex(history.current());
        history.disable = true;
        selectItem(p.first, p.second, -1);
        history.disable = false;
    }
}

void MainWindow::on_actionForward_triggered()
{
    if(history.forward())
    {
        auto p = model_->decodeIndex(history.current());
        history.disable = true;
        selectItem(p.first, p.second, -1);
        history.disable = false;
    }
}

bool MainWindow::History::back()
{
    if(index.empty())
    {
        position = -1;
        return false;
    }
    int newPos = qMax(0, qMin(index.size() - 1, position - 1));
    bool changed = newPos != position;
    position = newPos;
    updateActions();
    return changed;
}

bool MainWindow::History::forward()
{
    if(index.empty())
    {
        position = -1;
        return false;
    }
    int newPos = qMax(0, qMin(index.size() - 1, position + 1));
    bool changed = newPos != position;
    position = newPos;
    updateActions();
    return changed;
}

void MainWindow::History::push(QModelIndex i)
{
    if(disable) return;
    index.append(i);
    position = index.size() - 1;
    updateActions();
}

void MainWindow::History::updateActions()
{
    mainWindow->ui->actionBack->setEnabled(position > 0 && index.size() > 1);
    mainWindow->ui->actionForward->setEnabled(position < index.size() - 1 && index.size() > 1);
}

void MainWindow::History::clear()
{
    index.clear();
    position = -1;
    updateActions();
}

QModelIndex MainWindow::History::current()
{
    if(index.empty() || position < 0 || position >= index.size()) return {};
    return index.at(position);
}
