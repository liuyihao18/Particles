#include "sphere.h"
#include "constant.h"

#include <QVector>

Sphere::Sphere(float mass, float radius, const Material& material)
	: mass(mass), radius(radius), material(material)
{
    
}

Sphere::~Sphere()
{
}

// Reference: https://blog.csdn.net/weixin_41234001/article/details/104701508
void Sphere::GetVertices(QVector<float>& sphereVertices)
{
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

// Reference: https://blog.csdn.net/weixin_41234001/article/details/104701508
void Sphere::GetIndices(QVector<unsigned int>& sphereIndices)
{
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

Sphere Sphere::GetProto(unsigned int type)
{
    switch (type) {
    case 0:
        return Sphere(
            1.0f, 1.0f / 32.0f, Material::ruby()
        );
    case 1:
        return Sphere(
            1.0f, 1.0f / 32.0f, Material::emerald()
        );
    case 2:
        return Sphere(
            1.5f, 1.0f / 48.0f, Material::silver()
        );
    case 3:
        return Sphere(
            2.0f, 1.0f / 64.0f, Material::gold()
        );
    }
    return Sphere(
        1.0f, 1.0f, Material::ruby()
    );
}
