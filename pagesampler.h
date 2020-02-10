#ifndef PAGESAMPLER_H
#define PAGESAMPLER_H

#include "page.h"

namespace Ui {
class PageSampler;
}

class PageSampler : public Page
{
    Q_OBJECT

public:
    explicit PageSampler(MainWindow *mainWindow);
    ~PageSampler();

    void setData(const tinygltf::Model &model, int index);

private:
    Ui::PageSampler *ui;
};

#endif // PAGESAMPLER_H
