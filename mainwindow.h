#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gltfmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Page;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    GLTFModel * model() { return model_; }

    static bool decodeLink(const QString &link, GLTFModel::Group &group, int &index);
    static QString link(GLTFModel::Group group, int index);

private slots:
    void onItemSelected(const QModelIndex &index, const QModelIndex &prev);

public slots:
    void selectItem(GLTFModel::Group group, int index, int subIndex = -1, bool syncTree = true);
    void selectItemByLink(const QString &link);

private:
    Ui::MainWindow *ui;
    GLTFModel *model_;
    QMap<GLTFModel::Group, Page*> page;
};

#endif // MAINWINDOW_H
