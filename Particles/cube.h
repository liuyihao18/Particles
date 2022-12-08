#pragma once

#include <QVector3D>

// This cube is just used to hold the sphere.
class Cube
{
private:
	Cube() = default;
	Cube(const QVector3D& position, float size);
public:
	~Cube();

public:
	QVector3D position;
	float size{ 1.0f };
	
public:
	static constexpr char VERT_PATH[] = ":/shader/cube.vert";
	static constexpr char FRAG_PATH[] = ":/shader/cube.frag";
	static void GetVertices(QVector<float>& cubeVertices);
	static void GetIndices(QVector<unsigned int>& cubeIndices);
	static Cube GetContainer();
};

