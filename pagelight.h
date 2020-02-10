#ifndef PAGELIGHT_H
#define PAGELIGHT_H

#include "page.h"

namespace Ui {
class PageLight;
}

class PageLight : public Page
{
    Q_OBJECT

public:
    explicit PageLight(MainWindow *mainWindow);
    ~PageLight();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageLight *ui;
};

#endif // PAGELIGHT_H
