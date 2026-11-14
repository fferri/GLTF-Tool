#include "gltfinfo.h"
#include "tiny_gltf.h"

#include <climits>

QMap<int, QStringList> GLTFInfo::typeFields {
    {TINYGLTF_TYPE_SCALAR, {"v"}},
    {TINYGLTF_TYPE_VEC2, {"x", "y"}},
    {TINYGLTF_TYPE_VEC3, {"x", "y", "z"}},
    {TINYGLTF_TYPE_VEC4, {"x", "y", "z", "w"}},
    {TINYGLTF_TYPE_MAT2, {"m11", "m12", "m21", "m22"}},
    {TINYGLTF_TYPE_MAT3, {"m11", "m12", "m13", "m21", "m22", "m23", "m31", "m32", "m33"}},
    {TINYGLTF_TYPE_MAT4, {"m11", "m12", "m13", "m14", "m21", "m22", "m23", "m24", "m31", "m32", "m33", "m34", "m41", "m42", "m43", "m44"}},
};

template<typename T>
static QStringList l(const T *b, int n)
{
    QStringList r;
    for(int i = 0; i < n; i++) r << QString::number(b[i]);
    return r;
}

QMap<int, std::function<QStringList(const char*,int)>> GLTFInfo::cells {
    {TINYGLTF_COMPONENT_TYPE_BYTE,           [] (const char *b, int n) {return l(reinterpret_cast<const char*>(b), n);}},
    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,  [] (const char *b, int n) {return l(reinterpret_cast<const unsigned char*>(b), n);}},
    {TINYGLTF_COMPONENT_TYPE_SHORT,          [] (const char *b, int n) {return l(reinterpret_cast<const short*>(b), n);}},
    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, [] (const char *b, int n) {return l(reinterpret_cast<const unsigned short*>(b), n);}},
    {TINYGLTF_COMPONENT_TYPE_INT,            [] (const char *b, int n) {return l(reinterpret_cast<const int*>(b), n);}},
    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,   [] (const char *b, int n) {return l(reinterpret_cast<const unsigned int*>(b), n);}},
    {TINYGLTF_COMPONENT_TYPE_FLOAT,          [] (const char *b, int n) {return l(reinterpret_cast<const float*>(b), n);}},
    {TINYGLTF_COMPONENT_TYPE_DOUBLE,         [] (const char *b, int n) {return l(reinterpret_cast<const double*>(b), n);}},
};

QMap<int, QString> GLTFInfo::typeStr {
    {TINYGLTF_TYPE_SCALAR, "scalar"},
    {TINYGLTF_TYPE_VEC2, "vec2"},
    {TINYGLTF_TYPE_VEC3, "vec3"},
    {TINYGLTF_TYPE_VEC4, "vec4"},
    {TINYGLTF_TYPE_MAT2, "mat2"},
    {TINYGLTF_TYPE_MAT3, "mat3"},
    {TINYGLTF_TYPE_MAT4, "mat4"},
};

QMap<int, QString> GLTFInfo::compTypeStr {
    {TINYGLTF_COMPONENT_TYPE_BYTE, "byte"},
    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, "unsigned byte"},
    {TINYGLTF_COMPONENT_TYPE_SHORT, "short"},
    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, "unsigned short"},
    {TINYGLTF_COMPONENT_TYPE_INT, "int"},
    {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, "unsigned int"},
    {TINYGLTF_COMPONENT_TYPE_FLOAT, "float"},
    {TINYGLTF_COMPONENT_TYPE_DOUBLE, "double"},
};

QByteArray GLTFInfo::getBuffer(const tinygltf::Model *model, int i)
{
    if(!model || i < 0 || i >= (int)model->buffers.size()) return {};
    const tinygltf::Buffer &buffer = model->buffers[i];
    int size = buffer.data.size() > (size_t)INT_MAX ? INT_MAX : (int)buffer.data.size();
    return QByteArray(reinterpret_cast<const char*>(buffer.data.data()), size);
}

QByteArray GLTFInfo::getBufferView(const tinygltf::Model *model, int i)
{
    if(!model || i < 0 || i >= (int)model->bufferViews.size()) return {};
    const tinygltf::BufferView &bufferView = model->bufferViews[i];
    if(bufferView.buffer < 0 || bufferView.buffer >= (int)model->buffers.size()) return {};

    const tinygltf::Buffer &buffer = model->buffers[bufferView.buffer];
    qint64 bufferSize = (qint64)buffer.data.size();
    qint64 start = qMin((qint64)bufferView.byteOffset, bufferSize);
    qint64 length = qMin((qint64)bufferView.byteLength, bufferSize - start);
    if(length <= 0) return {};
    if(length > INT_MAX) length = INT_MAX;
    return QByteArray(reinterpret_cast<const char*>(buffer.data.data()) + start, (int)length);
}

int GLTFInfo::componentCount(int type)
{
    return tinygltf::GetNumComponentsInType(static_cast<uint32_t>(type));
}

int GLTFInfo::componentOffset(int type, int componentType, int component)
{
    int componentSize = tinygltf::GetComponentSizeInBytes(static_cast<uint32_t>(componentType));
    if(componentSize <= 0 || component < 0) return -1;

    int rows = 0;
    switch(type)
    {
    case TINYGLTF_TYPE_MAT2: rows = 2; break;
    case TINYGLTF_TYPE_MAT3: rows = 3; break;
    case TINYGLTF_TYPE_MAT4: rows = 4; break;
    default: return component * componentSize;
    }

    // Matrix elements are stored column-major; each column is padded to a
    // 4-byte boundary when the component type is 1 or 2 bytes wide.
    int column = component / rows;
    int row = component % rows;
    int columnBytes = rows * componentSize;
    int columnStride = (columnBytes + 3) & ~3;
    return column * columnStride + row * componentSize;
}

int GLTFInfo::elementSize(int type, int componentType)
{
    int componentSize = tinygltf::GetComponentSizeInBytes(static_cast<uint32_t>(componentType));
    int components = componentCount(type);
    if(componentSize <= 0 || components <= 0) return 0;

    switch(type)
    {
    case TINYGLTF_TYPE_MAT2: return 2 * (((2 * componentSize) + 3) & ~3);
    case TINYGLTF_TYPE_MAT3: return 3 * (((3 * componentSize) + 3) & ~3);
    case TINYGLTF_TYPE_MAT4: return 4 * (((4 * componentSize) + 3) & ~3);
    default: return components * componentSize;
    }
}

QVector<QStringList> GLTFInfo::accessorData(const tinygltf::Model *model, int i)
{
    QVector<QStringList> rows;
    if(!model || i < 0 || i >= (int)model->accessors.size()) return rows;

    const tinygltf::Accessor &accessor = model->accessors[i];
    int components = componentCount(accessor.type);
    int componentSize = tinygltf::GetComponentSizeInBytes(static_cast<uint32_t>(accessor.componentType));
    int elementBytes = elementSize(accessor.type, accessor.componentType);
    if(components <= 0 || componentSize <= 0 || elementBytes <= 0) return rows;
    if(accessor.count > (size_t)INT_MAX) return rows;

    int count = (int)accessor.count;

    // An accessor without a bufferView reads as all zeroes (sparse accessors
    // are not supported by this version of tinygltf).
    if(accessor.bufferView < 0 || accessor.bufferView >= (int)model->bufferViews.size())
    {
        QStringList zeroRow;
        for(int c = 0; c < components; c++) zeroRow << "0";
        rows.reserve(count);
        for(int row = 0; row < count; row++) rows.append(zeroRow);
        return rows;
    }

    const tinygltf::BufferView &bufferView = model->bufferViews[accessor.bufferView];
    if(bufferView.buffer < 0 || bufferView.buffer >= (int)model->buffers.size()) return rows;

    const tinygltf::Buffer &buffer = model->buffers[bufferView.buffer];
    const char *base = reinterpret_cast<const char*>(buffer.data.data());
    qint64 bufferSize = (qint64)buffer.data.size();
    qint64 viewStart = (qint64)bufferView.byteOffset;
    qint64 viewEnd = viewStart + (qint64)bufferView.byteLength;
    qint64 elementsStart = viewStart + (qint64)accessor.byteOffset;
    qint64 stride = bufferView.byteStride > 0 ? (qint64)bufferView.byteStride : (qint64)elementBytes;
    if(stride < elementBytes) stride = elementBytes;

    auto cell = cells.value(accessor.componentType);
    if(!cell) return rows;

    rows.reserve(count);
    for(int row = 0; row < count; row++)
    {
        qint64 elementStart = elementsStart + row * stride;
        if(elementStart < viewStart || elementStart + elementBytes > viewEnd ||
           elementStart + elementBytes > bufferSize)
            break;

        QStringList values;
        for(int component = 0; component < components; component++)
        {
            int offset = componentOffset(accessor.type, accessor.componentType, component);
            if(offset < 0) { values << QString(); continue; }
            values << cell(base + elementStart + offset, 1).value(0);
        }
        rows.append(values);
    }
    return rows;
}

QVector<GLTFInfo::AnimationTrack> GLTFInfo::animationTracks(const tinygltf::Model *model, int i)
{
    QVector<AnimationTrack> tracks;
    if(!model || i < 0 || i >= (int)model->animations.size()) return tracks;

    const tinygltf::Animation &animation = model->animations[i];
    if(animation.channels.size() > (size_t)INT_MAX) return tracks;
    tracks.reserve((int)animation.channels.size());

    for(int channelIndex = 0; channelIndex < (int)animation.channels.size(); channelIndex++)
    {
        const tinygltf::AnimationChannel &channel = animation.channels[channelIndex];

        AnimationTrack track;
        track.channel = channelIndex;
        track.node = channel.target_node;
        track.path = QString::fromStdString(channel.target_path);
        track.sampler = channel.sampler;

        if(channel.sampler >= 0 && channel.sampler < (int)animation.samplers.size())
        {
            const tinygltf::AnimationSampler &sampler = animation.samplers[channel.sampler];
            track.interpolation = QString::fromStdString(sampler.interpolation);
            if(track.interpolation.isEmpty()) track.interpolation = "LINEAR"; // glTF default
            track.inputAccessor = sampler.input;
            track.outputAccessor = sampler.output;

            if(sampler.input >= 0 && sampler.input < (int)model->accessors.size())
            {
                const tinygltf::Accessor &input = model->accessors[sampler.input];
                track.keyCount = (int)qMin<size_t>(input.count, (size_t)INT_MAX);
                if(!input.minValues.empty() && !input.maxValues.empty())
                {
                    track.timeMin = input.minValues[0];
                    track.timeMax = input.maxValues[0];
                    track.hasTimeRange = true;
                }
            }
        }

        tracks.append(track);
    }

    return tracks;
}
