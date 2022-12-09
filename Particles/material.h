#pragma once

#include <QVector3D>

class Material
{
public:
	Material() = default;
	Material(const QVector3D& ambient, const QVector3D& diffuse, const QVector3D& specular, float shininess);
	~Material();

public:
	QVector3D ambient;
	QVector3D diffuse;
	QVector3D specular;
	float shininess{ 1.0f };

public:
	// The following materials are from: 
	// http://devernay.free.fr/cours/opengl/materials.html
	static Material ruby();
	static Material emerald();
	static Material silver();
	static Material gold();
	static Material random();
};

