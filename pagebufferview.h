#ifndef PAGEBUFFERVIEW_H
#define PAGEBUFFERVIEW_H

#include "page.h"

namespace Ui {
class PageBufferView;
}

class PageBufferView : public Page
{
    Q_OBJECT

public:
    explicit PageBufferView(MainWindow *mainWindow);
    ~PageBufferView();

    void setData(const tinygltf::Model &model, int index, int subIndex = -1);

private:
    Ui::PageBufferView *ui;
};

#endif // PAGEBUFFERVIEW_H
