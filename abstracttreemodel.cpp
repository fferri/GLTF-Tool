#include "abstracttreemodel.h"

AbstractTreeModel::AbstractTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

AbstractTreeModel::~AbstractTreeModel()
{
    for(auto v : indexData_) delete v;
}

QModelIndex AbstractTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    QString dataKey = indexString(row, column, parent);
    auto it = indexData_.find(dataKey);
    IndexData *data;
    if(it == indexData_.end())
    {
        data = new IndexData;
        const_cast<AbstractTreeModel*>(this)->indexData_.insert(dataKey, data);
        if(parent.isValid())
        {
            data->parents.append(parent);
            data->parents.append(indexData(parent)->parents);
        }
    }
    else
    {
        data = it.value();
    }
    return createIndex(row, column, data);
}

QModelIndex AbstractTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid()) return {};
    auto data = indexData(index);
    if(data->parents.empty()) return {};
    return data->parents.at(0);
}

AbstractTreeModel::IndexData * AbstractTreeModel::indexData(const QModelIndex &index) const
{
    if(!index.internalPointer()) return nullptr;
    return reinterpret_cast<IndexData*>(index.internalPointer());
}

QList<int> AbstractTreeModel::indexPath(const QModelIndex &index) const
{
    QList<int> path;
    if(index.isValid())
    {
        auto data = indexData(index);
        for(int i = data->parents.size() - 1; i >= 0; i--)
            path.append(data->parents[i].row());
        path.append(index.row());
    }
    return path;
}

QString AbstractTreeModel::indexString(const QModelIndex &index) const
{
    return indexString(index.row(), index.column(), index.parent());
}

QString AbstractTreeModel::indexString(int row, int column, const QModelIndex &parent) const
{
    QString pre = parent.isValid() ? indexString(parent) + "." : "";
    return pre + QString("[%1,%2]").arg(row).arg(column);
}
