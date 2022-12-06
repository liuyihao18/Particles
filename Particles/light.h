#pragma once

#include <QVector3D>

class Light
{
public:
	Light() = default;
	Light(const QVector3D& position, const QVector3D& color, float ambientDecay, float diffuseDecay);
	~Light();

public:
	QVector3D ambient() const;
	QVector3D diffuse() const;
	QVector3D specular() const;

public:
	QVector3D position;
	QVector3D color;
	float ambientDecay{ 0 };
	float diffuseDecay{ 0 };
	float size{ 0.1f };

public:
	static constexpr char VERT_PATH[] = ":/shader/light.vert";
	static constexpr char FRAG_PATH[] = ":/shader/light.frag";
	static void GetVertices(QVector<float>& lightVertices);
	static void GetIndices(QVector<unsigned int>& lightIndices);
};

