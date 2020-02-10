#ifndef PAGENODE_H
#define PAGENODE_H

#include "page.h"

namespace Ui {
class PageNode;
}

class PageNode : public Page
{
    Q_OBJECT

public:
    explicit PageNode(MainWindow *mainWindow);
    ~PageNode();

    void setData(const tinygltf::Model &model, int index, int subIndex = -1);

private:
    Ui::PageNode *ui;
};

#endif // PAGENODE_H
