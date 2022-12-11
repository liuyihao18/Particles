#include "sphere.h"

Sphere *Sphere::protos[PROTO_NUM] = {nullptr};

Sphere::Sphere(float mass, float radius)
    : mass(mass), radius(radius)
{
}

Sphere::~Sphere()
{
}

// VBO
void Sphere::GetVertices(QVector<float> &sphereVertices)
{
    // Reference: https://blog.csdn.net/weixin_41234001/article/details/104701508
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
        }
    }
}

// EBO
void Sphere::GetIndices(QVector<unsigned int> &sphereIndices)
{
    // Reference: https://blog.csdn.net/weixin_41234001/article/details/104701508
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }
}

// 获取原型
Sphere *Sphere::GetProto(unsigned int type)
{
    switch (type)
    {
    case 0:
        if (!protos[type])
        {
            protos[type] = new Sphere(4.0f, 1.0f / 32.0f);
        }
        return protos[type];
    case 1:
        if (!protos[type])
        {
            protos[type] = new Sphere(1.5f, 1.0f / 40.0f);
        }
        return protos[type];
    case 2:
        if (!protos[type])
        {
            protos[type] = new Sphere(2.3f, 1.0f / 48.0f);
        }
        return protos[type];
    case 3:
        if (!protos[type])
        {
            protos[type] = new Sphere(2.7f, 1.0f / 64.0f);
        }
        return protos[type];
    case 4:
        if (!protos[type])
        {
            protos[type] = new Sphere(10.0f, 1.0f / 24.0f);
        }
        return protos[type];
    }
    return nullptr;
}
