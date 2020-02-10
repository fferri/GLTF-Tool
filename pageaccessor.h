#ifndef PAGEACCESSOR_H
#define PAGEACCESSOR_H

#include "page.h"

namespace Ui {
class PageAccessor;
}

class PageAccessor : public Page
{
    Q_OBJECT

public:
    explicit PageAccessor(MainWindow *mainWindow);
    ~PageAccessor();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageAccessor *ui;
};

#endif // PAGEACCESSOR_H
