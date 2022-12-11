#pragma once

#include <QVector3D>
#include <vector_types.h>
#include <random>

inline float randomFloat()
{
    static std::uniform_real_distribution<float> dis(0.0, 1.0);
    static std::mt19937 gen{std::random_device{}()};
    return dis(gen);
}

inline float randomFloat(float min, float max)
{
    return min + (max - min) * randomFloat();
}

inline QVector3D randomVector()
{
    return QVector3D(randomFloat(), randomFloat(), randomFloat());
}

inline float3 make_float3(const QVector3D &vec)
{
    return {vec.x(), vec.y(), vec.z()};
}

inline QString vecToString(const QVector3D &vec) 
{
    QString str;
    str += "(";
    str += QString::number(vec.x(), 'f', 2);
    str += ", ";
    str += QString::number(vec.y(), 'f', 2);
    str += ", ";
    str += QString::number(vec.z(), 'f', 2);
    str += ")";
    return str;
}
