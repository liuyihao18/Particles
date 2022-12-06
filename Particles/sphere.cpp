#include "sphere.h"
#include "constant.h"

#include <QVector>

Sphere::Sphere(const QVector3D& position, float radius, const Material& material)
	: position(position), radius(radius), material(material)
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
            float xPos = 0.5 * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = 0.5 * std::cos(ySegment * PI);
            float zPos = 0.5 * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
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
