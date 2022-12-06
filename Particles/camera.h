#pragma once

#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QVector3D>

class Camera
{
public:
	Camera();
	Camera(const QVector3D& eye, const QVector3D& up, const QVector3D& center);
	Camera(const QVector3D& eye, const QVector3D& up, const QVector3D& center, float disR);
	~Camera();

private:
	QVector3D eye;
	QVector3D up;
	QVector3D center;
	float disR;

public:
	QMatrix4x4 getViewMatrix() const;
	void translateLeft(float dis);
	void translateUp(float dis);
	void translateForward(float dis);
	void rotateLeft(float degree);
	void rotateUp(float degree);
};

