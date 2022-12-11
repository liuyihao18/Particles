#pragma once

#include "constant.h"

#include <QVector3D>
#include <QVector>

constexpr int Y_SEGMENTS = 50; // 沿Y方向的分段数
constexpr int X_SEGMENTS = 50; // 沿X方向的分段数

class Sphere
{
private:
    Sphere() = default;
    Sphere(float mass, float radius);

public:
    ~Sphere();

public:
    float mass{1.0f};   // 质量
    float radius{1.0f}; // 半径

public:
    /* 着色器 */
    static constexpr char VERT_PATH[] = ":/shader/sphere.vert"; // 顶点着色器
    static constexpr char FRAG_PATH[] = ":/shader/sphere.frag"; // 片段着色器
    /* 顶点信息 */
    static void GetVertices(QVector<float> &sphereVertices);      // VBO
    static void GetIndices(QVector<unsigned int> &sphereIndices); // EBO
    // 原型
    static Sphere *protos[PROTO_NUM];
    static Sphere *GetProto(unsigned int type);
};
