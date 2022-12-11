#pragma once

#include <QVector3D>

class Material
{
public:
    Material() = default;
    Material(const QVector3D &ambient, const QVector3D &diffuse, const QVector3D &specular, float shininess);
    ~Material();

public:
    QVector3D ambient;     // 环境反射
    QVector3D diffuse;     // 漫反射
    QVector3D specular;    // 镜面反射
    float shininess{1.0f}; // 高光度

public:
    // 材质参考：
    // http://devernay.free.fr/cours/opengl/materials.html
    static Material ruby();
    static Material emerald();
    static Material silver();
    static Material gold();
    static Material random();
};
