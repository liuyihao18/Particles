#pragma once

#include "constant.h"

#include <QVector3D>
#include <QVector>

constexpr int Y_SEGMENTS = 50;
constexpr int X_SEGMENTS = 50;

class Sphere
{
private:
    Sphere() = default;
    Sphere(float mass, float radius);
public:
    ~Sphere();

public:
    float mass{ 1.0f };
    float radius{ 1.0f };

public:
    static constexpr char VERT_PATH[] = ":/shader/sphere.vert";
    static constexpr char FRAG_PATH[] = ":/shader/sphere.frag";
    static void GetVertices(QVector<float>& sphereVertices);
    static void GetIndices(QVector<unsigned int>& sphereIndices);
    static Sphere* protos[PROTO_NUM];
    static Sphere* GetProto(unsigned int type);
};
