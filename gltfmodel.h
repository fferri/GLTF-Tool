#ifndef GLTFMODEL_H
#define GLTFMODEL_H

#include <QtCore>
#include "abstracttreemodel.h"
#include "tinygltf/tiny_gltf.h"

class GLTFModel : public AbstractTreeModel
{
    Q_OBJECT

private:
    struct IndexData
    {
        QVector<QModelIndex> parents;
    };

public:
    enum Group
    {
        Accessors,
        Animations,
        Buffers,
        BufferViews,
        Materials,
        Meshes,
        Nodes,
        Textures,
        Images,
        Skins,
        Samplers,
        Cameras,
        Scenes,
        Lights,
        None = -1
    };
    Q_ENUM(Group)

public:
    explicit GLTFModel(QObject *parent = nullptr);
    ~GLTFModel();

    void setGLTF(tinygltf::Model *gltf);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QPair<Group, int> decodeIndex(const QModelIndex &index) const;
    QModelIndex encodeIndex(Group group, int idx) const;

private:
    tinygltf::Model *gltf_;
    QMap<int, int> nodeParent_;
    QMetaEnum groupMeta{QMetaEnum::fromType<Group>()};
};

#endif // GLTFMODEL_H
