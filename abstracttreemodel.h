#ifndef ABSTRACTTREEMODEL_H
#define ABSTRACTTREEMODEL_H

#include <QtCore>

class AbstractTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    struct IndexData
    {
        QVector<QModelIndex> parents;
    };

public:
    AbstractTreeModel();
    explicit AbstractTreeModel(QObject *parent = nullptr);
    ~AbstractTreeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    IndexData * indexData(const QModelIndex &index) const;
    QList<int> indexPath(const QModelIndex &index) const;
    QString indexString(const QModelIndex &index) const;
    QString indexString(int row, int column, const QModelIndex &parent) const;

private:
    QMap<QString, IndexData*> indexData_;
};

#endif // ABSTRACTTREEMODEL_H
