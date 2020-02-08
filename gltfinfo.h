#ifndef GLTFINFO_H
#define GLTFINFO_H

#include <QtCore>

struct GLTFInfo
{
    static QMap<int, QStringList> typeFields;
    static QMap<int, std::function<QStringList(const char*,int)>> cells;
    static QMap<int, QString> typeStr;
    static QMap<int, QString> compTypeStr;
};

#endif // GLTFINFO_H
