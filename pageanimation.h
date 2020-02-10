#ifndef PAGEANIMATION_H
#define PAGEANIMATION_H

#include "page.h"

namespace Ui {
class PageAnimation;
}

class PageAnimation : public Page
{
    Q_OBJECT

public:
    explicit PageAnimation(MainWindow *mainWindow);
    ~PageAnimation();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageAnimation *ui;
};

#endif // PAGEANIMATION_H
