#ifndef PAGESCENE_H
#define PAGESCENE_H

#include "page.h"

namespace Ui {
class PageScene;
}

class PageScene : public Page
{
    Q_OBJECT

public:
    explicit PageScene(MainWindow *mainWindow);
    ~PageScene();

    void setData(const tinygltf::Model &model, int index, int subIndex = -1);

private:
    Ui::PageScene *ui;
};

#endif // PAGESCENE_H
