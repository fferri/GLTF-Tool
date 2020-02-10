#ifndef PAGEIMAGE_H
#define PAGEIMAGE_H

#include "page.h"

namespace Ui {
class PageImage;
}

class PageImage : public Page
{
    Q_OBJECT

public:
    explicit PageImage(MainWindow *mainWindow);
    ~PageImage();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageImage *ui;
};

#endif // PAGEIMAGE_H
