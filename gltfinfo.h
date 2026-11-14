#ifndef GLTFINFO_H
#define GLTFINFO_H

#include <QtCore>

namespace tinygltf {
class Model;
}

struct GLTFInfo
{
    // One animation channel ("track") with its resolved sampler data.
    struct AnimationTrack
    {
        int channel = -1;
        int node = -1;
        QString path;
        int sampler = -1;
        QString interpolation;
        int inputAccessor = -1;
        int outputAccessor = -1;
        int keyCount = 0;
        bool hasTimeRange = false;
        double timeMin = 0;
        double timeMax = 0;
    };

    static QMap<int, QStringList> typeFields;
    static QMap<int, std::function<QStringList(const char*,int)>> cells;
    static QMap<int, QString> typeStr;
    static QMap<int, QString> compTypeStr;

    static QByteArray getBuffer(const tinygltf::Model *model, int i);
    static QByteArray getBufferView(const tinygltf::Model *model, int i);

    // Number of components in one element (e.g. 3 for VEC3).
    static int componentCount(int type);
    // Offset in bytes of component `component` inside one element.
    static int componentOffset(int type, int componentType, int component);
    // Size in bytes of one element, honoring matrix column padding.
    static int elementSize(int type, int componentType);

    // Decodes an accessor into one row of formatted component values per element.
    // Handles accessor.byteOffset, bufferView.byteStride and accessor.count, and
    // returns every element instead of assuming tightly packed data at offset 0.
    static QVector<QStringList> accessorData(const tinygltf::Model *model, int i);

    // Lists every channel of an animation (one entry per track).
    static QVector<AnimationTrack> animationTracks(const tinygltf::Model *model, int i);

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
