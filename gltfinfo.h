#ifndef GLTFINFO_H
#define GLTFINFO_H

#include <QtCore>

namespace tinygltf {
class Model;
}

struct GLTFInfo
{
    static QMap<int, QStringList> typeFields;
    static QMap<int, std::function<QStringList(const char*,int)>> cells;
    static QMap<int, QString> typeStr;
    static QMap<int, QString> compTypeStr;

    static QByteArray getBuffer(const tinygltf::Model *model, int i);
    static QByteArray getBufferView(const tinygltf::Model *model, int i);

    template<typename T>
    static QString vec2str(const std::vector<T> &v)
    {
        QStringList s;
        for(auto x : v) s << QString::number(x);
        return s.join(" ");
    }

    template<typename T>
    static QString mat2str(const std::vector<T> &v, int cols = 4)
    {
        QStringList s;
        for(int i = 0; i < v.size(); i += cols)
        {
            QStringList t;
            for(int j = 0; j < cols; j++)
                t << QString::number(v[i + j]);
            s << t.join(" ");
        }
        return s.join("\n");
    }
};

#endif // GLTFINFO_H
