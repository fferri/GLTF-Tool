#ifndef PAGECAMERA_H
#define PAGECAMERA_H

#include "page.h"

namespace Ui {
class PageCamera;
}

class PageCamera : public Page
{
    Q_OBJECT

public:
    explicit PageCamera(MainWindow *mainWindow);
    ~PageCamera();

    void setData(const tinygltf::Model &model, int index, int subIndex = -1);

private:
    Ui::PageCamera *ui;
};

#endif // PAGECAMERA_H
