QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abstracttreemodel.cpp \
    gltfinfo.cpp \
    gltfmodel.cpp \
    main.cpp \
    mainwindow.cpp \
    page.cpp \
    pageaccessor.cpp \
    pageanimation.cpp \
    pagebuffer.cpp \
    pagebufferview.cpp \
    pagecamera.cpp \
    pageimage.cpp \
    pagelight.cpp \
    pagematerial.cpp \
    pagemesh.cpp \
    pagenode.cpp \
    pagesampler.cpp \
    pagescene.cpp \
    pageskin.cpp \
    pagetexture.cpp \
    qhexedit2/src/chunks.cpp \
    qhexedit2/src/commands.cpp \
    qhexedit2/src/qhexedit.cpp \
    tinygltf.cpp

HEADERS += \
    abstracttreemodel.h \
    gltfinfo.h \
    gltfmodel.h \
    mainwindow.h \
    page.h \
    pageaccessor.h \
    pageanimation.h \
    pagebuffer.h \
    pagebufferview.h \
    pagecamera.h \
    pageimage.h \
    pagelight.h \
    pagematerial.h \
    pagemesh.h \
    pagenode.h \
    pagesampler.h \
    pagescene.h \
    pageskin.h \
    pagetexture.h \
    qhexedit2/src/chunks.h \
    qhexedit2/src/commands.h \
    qhexedit2/src/qhexedit.h \
    tinygltf/tiny_gltf.h

FORMS += \
    mainwindow.ui \
    pageaccessor.ui \
    pageanimation.ui \
    pagebuffer.ui \
    pagebufferview.ui \
    pagecamera.ui \
    pageimage.ui \
    pagelight.ui \
    pagematerial.ui \
    pagemesh.ui \
    pagenode.ui \
    pagesampler.ui \
    pagescene.ui \
    pageskin.ui \
    pagetexture.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
