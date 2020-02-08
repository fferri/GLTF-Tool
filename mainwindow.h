#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gltfmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onItemSelected(const QModelIndex &index, const QModelIndex &prev);
    void selectItem(GLTFModel::Group group, int index, bool syncTree = true);
    void selectItemByLink(const QString &link);

public slots:
    void showAccessor(int index);
    void showAnimation(int index);
    void showBuffer(int index);
    void showBufferView(int index);
    void showMaterial(int index);
    void showMesh(int index);
    void showNode(int index);
    void showTexture(int index);
    void showImage(int index);
    void showSkin(int index);
    void showSampler(int index);
    void showCamera(int index);
    void showScene(int index);
    void showLight(int index);

private:
    Ui::MainWindow *ui;
    GLTFModel *model;
    QMap<GLTFModel::Group, void (MainWindow::*)(int)> showFunc;
};

#endif // MAINWINDOW_H
