#pragma once

#include <QVector3D>

class Light
{
public:
    Light() = default;
    Light(const QVector3D &position, const QVector3D &color, float ambientDecay, float diffuseDecay);
    ~Light();

public:
    QVector3D ambient() const;  // 环境反射
    QVector3D diffuse() const;  // 漫反射
    QVector3D specular() const; // 镜面反射

public:
    QVector3D position;
    QVector3D color;
    float ambientDecay{0};
    float diffuseDecay{0};
    float size{0.1f};

public:
    /* 着色器 */
    static constexpr char VERT_PATH[] = ":/shader/light.vert"; // 顶点着色器
    static constexpr char FRAG_PATH[] = ":/shader/light.frag"; // 片段着色器
    /* 顶点信息 */
    static void GetVertices(QVector<float> &lightVertices);      // VBO
    static void GetIndices(QVector<unsigned int> &lightIndices); // EBO
};
