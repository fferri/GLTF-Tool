#include "gltfmodel.h"

template<typename T>
void gltfCollectionInfo(const std::vector<T> &v, int &size, int i, QString &name)
{
    size = (int)v.size();
    if(i >= 0 && i < size)
        name = QString::fromStdString(v[i].name);
    else
        name.clear();
}

void gltfCollectionInfo(const tinygltf::Model *model, GLTFModel::Group g, int &size, int i, QString &name)
{
    size = 0;
    name.clear();
    if(!model) return;

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
    case GLTFModel::None:        return;
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
    beginResetModel();
    clearIndexCache();

    gltf_ = gltf;
    nodeParent_.clear();
    sceneRoots_.clear();

    if(gltf_)
    {
        if(!gltf_->scenes.empty())
        {
            int sceneIndex = gltf_->defaultScene;
            if(sceneIndex < 0 || sceneIndex >= (int)gltf_->scenes.size())
                sceneIndex = 0;
            for(int node : gltf_->scenes[sceneIndex].nodes)
            {
                if(node >= 0 && node < (int)gltf_->nodes.size() && !sceneRoots_.contains(node))
                    sceneRoots_.append(node);
            }
        }
        else
        {
            // No scene: expose all nodes that are not children of another node.
            QSet<int> children;
            for(const tinygltf::Node &node : gltf_->nodes)
                for(int child : node.children)
                    children.insert(child);
            for(int node = 0; node < (int)gltf_->nodes.size(); node++)
                if(!children.contains(node))
                    sceneRoots_.append(node);
        }

        for(int node : sceneRoots_)
            nodeParent_[node] = -1;

        QList<int> stack = sceneRoots_;
        while(!stack.isEmpty())
        {
            int node = stack.takeLast();
            for(int child : gltf_->nodes[node].children)
            {
                if(child < 0 || child >= (int)gltf_->nodes.size()) continue;
                if(nodeParent_.contains(child)) continue; // guard against cycles
                nodeParent_[child] = node;
                stack.append(child);
            }
        }
    }

    endResetModel();
}

bool GLTFModel::nodeIndexAtPath(const QList<int> &path, int &idx) const
{
    if(!gltf_ || path.size() < 2) return false;

    int rootRow = path[1];
    if(rootRow < 0 || rootRow >= sceneRoots_.size()) return false;
    idx = sceneRoots_[rootRow];
    if(idx < 0 || idx >= (int)gltf_->nodes.size()) return false;

    for(int i = 2; i < path.size(); i++)
    {
        if(path[i] < 0 || path[i] >= (int)gltf_->nodes[idx].children.size()) return false;
        idx = gltf_->nodes[idx].children[path[i]];
        if(idx < 0 || idx >= (int)gltf_->nodes.size()) return false;
    }
    return true;
}

QVariant GLTFModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole || !gltf_) return {};

    auto path = indexPath(index);
    int d = path.size();
    if(d == 0) return {};

    Group group = static_cast<Group>(path[0]);
    if(group < Accessors || group > Lights) return {};

    if(d == 1)
    {
        int size;
        QString name;
        gltfCollectionInfo(gltf_, group, size, -1, name);
        return QString("%1 (%2 items)").arg(groupMeta.key(path[0])).arg(size);
    }

    if(group == Nodes)
    {
        int idx;
        if(!nodeIndexAtPath(path, idx)) return {};
        return QString("%1: %2").arg(idx).arg(QString::fromStdString(gltf_->nodes[idx].name));
    }

    if(d == 2)
    {
        int size;
        QString name;
        gltfCollectionInfo(gltf_, group, size, path[1], name);
        if(path[1] < 0 || path[1] >= size) return {};
        return QString("%1: %2").arg(path[1]).arg(name);
    }

    return {};
}

int GLTFModel::rowCount(const QModelIndex &parent) const
{
    if(!gltf_) return 0;
    if(!parent.isValid()) return groupMeta.keyCount() - 1; // top-level items (- none)

    auto path = indexPath(parent);
    if(path.isEmpty()) return 0;

    Group group = static_cast<Group>(path[0]);
    if(group < Accessors || group > Lights) return 0;

    if(group == Nodes)
    {
        if(path.size() == 1) return sceneRoots_.size();
        int idx;
        if(!nodeIndexAtPath(path, idx)) return 0;
        return (int)gltf_->nodes[idx].children.size();
    }

    if(path.size() == 1)
    {
        int size;
        QString name;
        gltfCollectionInfo(gltf_, group, size, -1, name);
        return size;
    }

    return 0;
}

int GLTFModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
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
    if(!index.isValid() || !gltf_) return qMakePair(None, -1);

    auto path = indexPath(index);
    if(path.isEmpty()) return qMakePair(None, -1);

    Group group = static_cast<Group>(path[0]);
    if(group < Accessors || group > Lights) return qMakePair(None, -1);

    int idx = -1;

    if(group == Nodes)
    {
        int nodeIdx;
        if(nodeIndexAtPath(path, nodeIdx)) idx = nodeIdx;
    }
    else if(path.size() == 2)
    {
        idx = path[1];
    }

    return qMakePair(group, idx);
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
    if(!gltf_ || group < Accessors || group > Lights) return {};

    if(group == Nodes)
    {
        if(idx < 0 || idx >= (int)gltf_->nodes.size()) return {};

        QList<int> path;
        int node = idx;
        while(true)
        {
            int parent = nodeParent_.value(node, -1);
            if(parent < 0) break;
            int row = indexOf(node, gltf_->nodes[parent].children);
            if(row < 0) return {};
            path.push_front(row);
            node = parent;
        }

        int rootRow = sceneRoots_.indexOf(node);
        if(rootRow < 0) return {};
        path.push_front(rootRow);

        QModelIndex result = index(Nodes, 0);
        for(int row : path)
            result = index(row, 0, result);
        return result;
    }

    int size;
    QString name;
    gltfCollectionInfo(gltf_, group, size, -1, name);
    if(idx < 0 || idx >= size) return {};
    return index(idx, 0, index(group, 0));
}
