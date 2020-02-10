#ifndef PAGETEXTURE_H
#define PAGETEXTURE_H

#include "page.h"

namespace Ui {
class PageTexture;
}

class PageTexture : public Page
{
    Q_OBJECT

public:
    explicit PageTexture(MainWindow *mainWindow);
    ~PageTexture();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageTexture *ui;
};

#endif // PAGETEXTURE_H
