#ifndef PAGE_H
#define PAGE_H

#include <QWidget>

namespace tinygltf {
class Model;
}

class MainWindow;

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(MainWindow *mainWindow);

    MainWindow * mainWindow() const;

    virtual void setData(const tinygltf::Model &model, int index) = 0;

signals:

private:
    MainWindow *mainWindow_;
};

#endif // PAGE_H
