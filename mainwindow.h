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

    void openFile(const QString &filePath);

    static bool decodeLink(const QString &link, GLTFModel::Group &group, int &index);
    static QString link(GLTFModel::Group group, int index);

private slots:
    void onItemSelected(const QModelIndex &index, const QModelIndex &prev);
    void on_actionOpen_triggered();
    void on_actionBack_triggered();
    void on_actionForward_triggered();

public slots:
    void selectItem(GLTFModel::Group group, int index);
    void selectItemByLink(const QString &link);

private:
    Ui::MainWindow *ui;
    GLTFModel *model_;
    QMap<GLTFModel::Group, Page*> page;
    struct History
    {
        MainWindow *mainWindow;
        bool back();
        bool forward();
        void push(QModelIndex i);
        void updateActions();
        void clear();
        QModelIndex current();
        bool disable{false};
    private:
        QVector<QModelIndex> index;
        int position;
    } history;
    friend class History;
};

#endif // MAINWINDOW_H
