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

    void setData(const tinygltf::Model &model, int index, int subIndex = -1);

private:
    Ui::PageMesh *ui;
};

#endif // PAGEMESH_H
