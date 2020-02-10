#ifndef PAGEBUFFER_H
#define PAGEBUFFER_H

#include "page.h"

namespace Ui {
class PageBuffer;
}

class PageBuffer : public Page
{
    Q_OBJECT

public:
    explicit PageBuffer(MainWindow *mainWindow);
    ~PageBuffer();

    void setData(const tinygltf::Model &model, int index, int subIndex = -1);

private:
    Ui::PageBuffer *ui;
};

#endif // PAGEBUFFER_H
