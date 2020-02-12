#include "gltfmodel.h"

template<typename T>
void gltfCollectionInfo(const std::vector<T> &v, int &size, int i, QString &name)
{
    size = v.size();
    if(i >= 0)
        name = QString::fromStdString(v[i].name);
}

void gltfCollectionInfo(const tinygltf::Model *model, GLTFModel::Group g, int &size, int i, QString &name)
{
    switch(g)
    {
    case GLTFModel::Accessors:   return gltfCollectionInfo(model->accessors, size, i, name);
    case GLTFModel::Animations:  return gltfCollectionInfo(model->animations, size, i, name);
    case GLTFModel::Buffers:     return gltfCollectionInfo(model->buffers, size, i, name);
    case GLTFModel::BufferViews: return gltfCollectionInfo(model->bufferViews, size, i, name);
    case GLTFModel::Materials:   return gltfCollectionInfo(model->materials, size, i, name);
    case GLTFModel::Meshes:      return gltfCollectionInfo(model->meshes, size, i, name);
    case GLTFModel::Nodes:       return gltfCollectionInfo(model->nodes, size, i, name);
    case GLTFModel::Textures:    return gltfCollectionInfo(model->textures, size, i, name);
    case GLTFModel::Images:      return gltfCollectionInfo(model->images, size, i, name);
    case GLTFModel::Skins:       return gltfCollectionInfo(model->skins, size, i, name);
    case GLTFModel::Samplers:    return gltfCollectionInfo(model->samplers, size, i, name);
    case GLTFModel::Cameras:     return gltfCollectionInfo(model->cameras, size, i, name);
    case GLTFModel::Scenes:      return gltfCollectionInfo(model->scenes, size, i, name);
    case GLTFModel::Lights:      return gltfCollectionInfo(model->lights, size, i, name);
    }
}

GLTFModel::GLTFModel(QObject *parent)
    : AbstractTreeModel(parent)
{
}

GLTFModel::~GLTFModel()
{
}

void GLTFModel::setGLTF(tinygltf::Model *gltf)
{
     gltf_ = gltf;

     nodeParent_.clear();
     QList<int> s;
     for(int node : gltf_->scenes[gltf_->defaultScene].nodes)
     {
         nodeParent_[node] = -1;
         s.append(node);
     }
     while(!s.empty())
     {
         int node = s.back(); s.pop_back();
         for(int child : gltf_->nodes[node].children)
         {
             nodeParent_[child] = node;
             s.append(child);
         }
     }
}

QVariant GLTFModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole) return {};

    auto path = indexPath(index);
    int d = path.size();

    if(d == 1)
    {
        int size;
        QString name;
        gltfCollectionInfo(gltf_, static_cast<GLTFModel::Group>(path[0]), size, -1, name);
        return QString("%1 (%2 items)").arg(groupMeta.key(path[0])).arg(size);
    }

    if(d == 2 && path[0] != Nodes)
    {
        int size;
        QString name;
        gltfCollectionInfo(gltf_, static_cast<GLTFModel::Group>(path[0]), size, path[1], name);
        return QString("%1: %2").arg(path[1]).arg(name);
    }

    if(d >= 2 && path[0] == Nodes)
    {
        int idx = gltf_->scenes[gltf_->defaultScene].nodes[path[1]];
        for(int i = 2; i < d; i++) idx = gltf_->nodes[idx].children[path[i]];
        return QString("%1: %2").arg(idx).arg(QString::fromStdString(gltf_->nodes[idx].name));
    }

    return {};
}

int GLTFModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) return groupMeta.keyCount() - 1; // top-level items (- none)

    auto path = indexPath(parent);
    int d = path.size();

    if(d == 1 && path[0] != Nodes)
    {
        int size;
        QString name;
        gltfCollectionInfo(gltf_, static_cast<GLTFModel::Group>(path[0]), size, -1, name);
        return size;
    }
    if(d >= 1 && path[0] == Nodes)
    {
        if(d == 1) return gltf_->scenes[gltf_->defaultScene].nodes.size();
        int idx = gltf_->scenes[gltf_->defaultScene].nodes[path[1]];
        for(int i = 2; i < d; i++) idx = gltf_->nodes[idx].children[path[i]];
        return gltf_->nodes[idx].children.size();
    }
    return 0;
}

int GLTFModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

Qt::ItemFlags GLTFModel::flags(const QModelIndex &index) const
{
    if(index.isValid()) return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return {};
}

QVariant GLTFModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) return {};
    if(orientation == Qt::Horizontal)
    {
        if(section == 0) return "Index";
    }
    return {};
}

QPair<GLTFModel::Group, int> GLTFModel::decodeIndex(const QModelIndex &index) const
{
    if(!index.isValid()) return qMakePair(None, -1);

    auto path = indexPath(index);
    int d = path.size();

    if(d == 0) return qMakePair(None, -1);

    Group g = static_cast<Group>(path[0]);
    int idx = -1;

    if(d == 2 && g != Nodes)
    {
        idx = path[1];
    }
    else if(d >= 2 && g == Nodes)
    {
        idx = gltf_->scenes[gltf_->defaultScene].nodes[path[1]];
        for(int i = 2; i < d; i++) idx = gltf_->nodes[idx].children[path[i]];
    }

    return qMakePair(g, idx);
}

template<typename T>
int indexOf(const T &t, const std::vector<T> &v)
{
    auto it = std::find(v.begin(), v.end(), t);
    if(it == v.end()) return -1;
    return std::distance(v.begin(), it);
}

QModelIndex GLTFModel::encodeIndex(Group group, int idx) const
{
    if(group == Nodes)
    {
        QList<int> path;
        while(1)
        {
            int p = nodeParent_[idx];
            if(p == -1) break;
            path.push_front(indexOf(idx, gltf_->nodes[p].children));
            idx = p;
        }
        {
            QModelIndex idx;
            for(int i = 0; i < path.length(); i++)
                idx = index(path[i], 0, idx);
            return idx;
        }
    }
    else return index(idx, 0, index(group, 0));
}
