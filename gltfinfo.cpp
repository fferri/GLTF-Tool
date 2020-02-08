#include "gltfinfo.h"
#include "tinygltf/tiny_gltf.h"

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

