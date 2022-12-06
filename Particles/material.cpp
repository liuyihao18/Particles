#include "material.h"

Material::Material(const QVector3D& ambient, const QVector3D& diffuse, const QVector3D& specular, float shininess)
	: ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
{
}

Material::~Material()
{
}
