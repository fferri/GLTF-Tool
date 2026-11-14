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

#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

#include <utility>

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
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionOpen->setToolTip(tr("Open a glTF file"));
    ui->actionOpen->setStatusTip(tr("Open a glTF file"));
    ui->toolBar->insertSeparator(ui->actionBack);
    for(auto p : page.values())
        ui->stackedWidget->addWidget(p);
    history.mainWindow = this;
    history.clear();

    setupRecentFilesMenu();
    loadRecentFiles();
    updateRecentFilesMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QString &filePath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool ret;
    std::string err, warn;
    tinygltf::Model loadedModel;
    if(filePath.endsWith(".gltf"))
        ret = loader.LoadASCIIFromFile(&loadedModel, &err, &warn, filePath.toStdString());
    else if(filePath.endsWith(".glb"))
        ret = loader.LoadBinaryFromFile(&loadedModel, &err, &warn, filePath.toStdString());
    else
    {
        QApplication::restoreOverrideCursor();
        return;
    }

    if(!ret)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("Load failed: " + filePath);
        msgBox.setInformativeText(QString::fromStdString(err));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    setWindowTitle(QString("%1 - GLTF-Tool").arg(QFileInfo(filePath).fileName()));
    setWindowFilePath(filePath);
    addRecentFile(filePath);

    if(!warn.empty())
    {
        QMessageBox msgBox;
        msgBox.setText("Load warning: " + filePath);
        msgBox.setInformativeText(QString::fromStdString(warn));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }

    if(model_)
    {
        model_->deleteLater();
        model_ = nullptr;
    }

    gltf = std::move(loadedModel);

    history.clear();
    model_= new GLTFModel(this);
    model_->setGLTF(&gltf);
    ui->treeView->setModel(model_);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onItemSelected);
    selectItem(GLTFModel::None, -1);

    QApplication::restoreOverrideCursor();
}

void MainWindow::setupRecentFilesMenu()
{
    ui->menuFile->addSeparator();
    recentFilesMenu_ = ui->menuFile->addMenu(tr("Open Recent"));
    recentFilesMenu_->setObjectName("menuRecentFiles");
    recentFilesMenu_->setToolTipsVisible(true);

    recentFileActions_.reserve(MaxRecentFiles);
    for(int i = 0; i < MaxRecentFiles; i++)
    {
        QAction *action = recentFilesMenu_->addAction(QString());
        action->setObjectName(QString("recentFileAction%1").arg(i));
        action->setVisible(false);
        connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
        recentFileActions_.append(action);
    }

    noRecentFilesAction_ = recentFilesMenu_->addAction(tr("(No recent files)"));
    noRecentFilesAction_->setObjectName("actionNoRecentFiles");
    noRecentFilesAction_->setEnabled(false);

    recentFilesMenu_->addSeparator();
    clearRecentFilesAction_ = recentFilesMenu_->addAction(tr("Clear Recent Files"));
    clearRecentFilesAction_->setObjectName("actionClearRecentFiles");
    connect(clearRecentFilesAction_, &QAction::triggered, this, &MainWindow::clearRecentFiles);
}

void MainWindow::loadRecentFiles()
{
    QSettings settings;
    recentFiles_ = settings.value("recentFiles").toStringList();

    QStringList unique;
    for(const QString &path : recentFiles_)
    {
        if(path.isEmpty() || unique.contains(path)) continue;
        unique.append(path);
    }
    recentFiles_ = unique;

    while(recentFiles_.size() > MaxRecentFiles) recentFiles_.removeLast();
}

void MainWindow::saveRecentFiles()
{
    QSettings settings;
    settings.setValue("recentFiles", recentFiles_);
}

void MainWindow::addRecentFile(const QString &filePath)
{
    if(filePath.isEmpty()) return;

    const QString path = QDir::cleanPath(QFileInfo(filePath).absoluteFilePath());
    recentFiles_.removeAll(path);
    recentFiles_.prepend(path);
    while(recentFiles_.size() > MaxRecentFiles) recentFiles_.removeLast();

    saveRecentFiles();
    updateRecentFilesMenu();
}

void MainWindow::updateRecentFilesMenu()
{
    for(int i = 0; i < recentFileActions_.size(); i++)
    {
        QAction *action = recentFileActions_[i];
        if(i < recentFiles_.size())
        {
            const QString &path = recentFiles_[i];
            QString label = QDir::toNativeSeparators(path);
            label.replace("&", "&&");
            action->setText(label);
            action->setToolTip(QDir::toNativeSeparators(path));
            action->setData(path);
            action->setVisible(true);
        }
        else
        {
            action->setVisible(false);
            action->setData(QVariant());
        }
    }

    noRecentFilesAction_->setVisible(recentFiles_.isEmpty());
    clearRecentFilesAction_->setEnabled(!recentFiles_.isEmpty());
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if(!action) return;

    const QString path = action->data().toString();
    if(path.isEmpty()) return;

    if(!QFileInfo::exists(path))
    {
        QMessageBox::warning(this, tr("File not found"),
                             tr("The file no longer exists:\n%1").arg(QDir::toNativeSeparators(path)));
        recentFiles_.removeAll(path);
        saveRecentFiles();
        updateRecentFilesMenu();
        return;
    }

    openFile(path);
}

void MainWindow::clearRecentFiles()
{
    recentFiles_.clear();
    saveRecentFiles();
    updateRecentFilesMenu();
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
    Q_UNUSED(prevModelIndex)
    if(!model_)
    {
        ui->stackedWidget->setCurrentWidget(ui->defaultPage);
        return;
    }

    auto p = model_->decodeIndex(modelIndex);
    GLTFModel::Group group = p.first;
    int index = p.second;

    history.push(modelIndex);
    auto w = page.value(group);
    if(w && index >= 0)
    {
        w->setData(gltf, index);
        ui->stackedWidget->setCurrentWidget(w);
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->defaultPage);
    }
}

void MainWindow::selectItem(GLTFModel::Group group, int index)
{
    QModelIndex modelIndex = model_->encodeIndex(group, index);
    if(!modelIndex.isValid())
    {
        ui->treeView->setCurrentIndex(QModelIndex());
        return;
    }

    // Make sure the item is visible, expanding all of its ancestors.
    for(QModelIndex parent = modelIndex.parent(); parent.isValid(); parent = parent.parent())
        ui->treeView->expand(parent);

    ui->treeView->setCurrentIndex(modelIndex);
    ui->treeView->scrollTo(modelIndex);
}

void MainWindow::selectItemByLink(const QString &link)
{
    GLTFModel::Group group;
    int index;
    if(decodeLink(link, group, index))
    {
        selectItem(group, index);
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
        selectItem(p.first, p.second);
        history.disable = false;
    }
}

void MainWindow::on_actionForward_triggered()
{
    if(history.forward())
    {
        auto p = model_->decodeIndex(history.current());
        history.disable = true;
        selectItem(p.first, p.second);
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
