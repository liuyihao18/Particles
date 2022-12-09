#include "material.h"
#include "utils.h"

Material::Material(const QVector3D& ambient, const QVector3D& diffuse, const QVector3D& specular, float shininess)
	: ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
{
}

Material::~Material()
{
}

Material Material::ruby()
{
	return Material(
		QVector3D(0.1745f, 0.01175f, 0.01175f),
		QVector3D(0.61424f, 0.04136f, 0.04136f),
		QVector3D(0.727811f, 0.626959f, 0.626959f),
		0.6f * 128.0f
	);
}

Material Material::emerald()
{
	return Material(
		QVector3D(0.0215f, 0.1745f, 0.0215f),
		QVector3D(0.07568f, 0.61424f, 0.07568f),
		QVector3D(0.633f, 0.727811f, 0.633f),
		0.6f * 128.0f
	);
}

Material Material::silver()
{
	return Material(
		QVector3D(0.19225f, 0.19225f, 0.19225f),
		QVector3D(0.50754f, 0.50754f, 0.50754f),
		QVector3D(0.508273f, 0.508273f, 0.508273f),
		0.4f * 128.0f
	);
}

Material Material::gold()
{
	return Material(
		QVector3D(0.24725f, 0.1995f, 0.0745f),
		QVector3D(0.75164f, 0.60648f, 0.22648f),
		QVector3D(0.628281f, 0.555802f, 0.366065f),
		0.4f * 128.0f
	);
}

Material Material::random()
{
	return Material(
		randomVector(),
		randomVector(),
		randomVector(),
		randomFloat(16.0f, 64.0f)
	);
}
