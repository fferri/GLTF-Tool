#ifndef PAGEMATERIAL_H
#define PAGEMATERIAL_H

#include "page.h"

namespace Ui {
class PageMaterial;
}

class PageMaterial : public Page
{
    Q_OBJECT

public:
    explicit PageMaterial(MainWindow *mainWindow);
    ~PageMaterial();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageMaterial *ui;
};

#endif // PAGEMATERIAL_H
