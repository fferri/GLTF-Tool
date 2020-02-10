#ifndef PAGEMESH_H
#define PAGEMESH_H

#include "page.h"

namespace Ui {
class PageMesh;
}

class PageMesh : public Page
{
    Q_OBJECT

public:
    explicit PageMesh(MainWindow *mainWindow);
    ~PageMesh();

    void setData(const tinygltf::Model &model, int index);

private slots:
    void on_primitive_currentIndexChanged(int index);

private:
    Ui::PageMesh *ui;
    const tinygltf::Model *model_;
    int index_;
};

#endif // PAGEMESH_H
