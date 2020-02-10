#ifndef PAGESKIN_H
#define PAGESKIN_H

#include "page.h"

namespace Ui {
class PageSkin;
}

class PageSkin : public Page
{
    Q_OBJECT

public:
    explicit PageSkin(MainWindow *mainWindow);
    ~PageSkin();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageSkin *ui;
};

#endif // PAGESKIN_H
