#pragma once

#include "material.h"

#include <QVector3D>

constexpr int Y_SEGMENTS = 50;
constexpr int X_SEGMENTS = 50;

class Sphere
{
private:
	Sphere() = default;
	Sphere(float mass, float radius, const Material& material);
public:
	~Sphere();

public:
	float mass{ 1.0f };
	float radius{ 1.0f };
	Material material;

public:
	static constexpr char VERT_PATH[] = ":/shader/sphere.vert";
	static constexpr char FRAG_PATH[] = ":/shader/sphere.frag";
	static void GetVertices(QVector<float>& sphereVertices);
	static void GetIndices(QVector<unsigned int>& sphereIndices);
	static Sphere GetProto(unsigned int type);
};
