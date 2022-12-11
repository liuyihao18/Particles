#include "camera.h"

Camera::Camera()
    : eye(0, 0, 1), up(0, 1, 0), center(0, 0, 0), disR(3.0)
{
}

Camera::Camera(const QVector3D &eye, const QVector3D &up, const QVector3D &lookat)
    : Camera(eye, up, lookat, 3.0)
{
}

Camera::Camera(const QVector3D &eye, const QVector3D &up, const QVector3D &lookat, float disR)
    : eye(eye), up(up.normalized()), center(lookat), disR(disR)
{
}

Camera::~Camera()
{
}

QMatrix4x4 Camera::getViewMatrix() const
{
    QVector3D real_eye = (eye - center).normalized() * disR + center;
    QMatrix4x4 matrix;
    matrix.lookAt(real_eye, center, up);
    return matrix;
}

QVector3D Camera::getEye() const
{
    return eye;
}

QVector3D Camera::getUp() const
{
    return up;
}

void Camera::translateLeft(float dis)
{
    QVector3D z = (eye - center).normalized();
    QVector3D x = QVector3D::crossProduct(up, z).normalized();
    eye += dis * -x;
    center += dis * -x;
}

void Camera::translateUp(float dis)
{
    QVector3D y = up.normalized();
    eye += dis * y;
    center += dis * y;
}

void Camera::translateForward(float dis)
{
    QVector3D z = (eye - center).normalized();
    eye -= dis * z;
    center -= dis * z;
}

void Camera::rotateLeft(float degree)
{
    eye = eye - center;
    QMatrix4x4 matrix;
    matrix.rotate(degree, up);
    eye = matrix.map(eye);
    eye = eye + center;
}

void Camera::rotateUp(float degree)
{
    eye = eye - center;
    QVector3D x = QVector3D::crossProduct(up, eye).normalized();
    QMatrix4x4 matrix;
    matrix.rotate(degree, x);
    eye = matrix.map(eye);
    up = QVector3D::crossProduct(eye, x).normalized();
    eye = eye + center;
}
