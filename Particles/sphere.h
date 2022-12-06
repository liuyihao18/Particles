#pragma once

#include "material.h"

#include <QVector3D>

constexpr int Y_SEGMENTS = 50;
constexpr int X_SEGMENTS = 50;

class Sphere
{
public:
	Sphere() = default;
	Sphere(const QVector3D& position, float radius, const Material& material);
	~Sphere();

public:
	QVector3D position;
	float radius;
	Material material;

public:
	static constexpr char VERT_PATH[] = ":/shader/sphere.vert";
	static constexpr char FRAG_PATH[] = ":/shader/sphere.frag";
	static void GetVertices(QVector<float>& sphereVertices);
	static void GetIndices(QVector<unsigned int>& sphereIndices);
};

