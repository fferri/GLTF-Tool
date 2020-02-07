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

private:
    Ui::MainWindow *ui;
    GLTFModel *model;
};

#endif // MAINWINDOW_H
