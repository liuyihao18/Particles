﻿#pragma once

#include "camera.h"

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QTimer>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	OpenGLWidget(QWidget* parent);
	~OpenGLWidget();

protected:
	void initializeGL() override;
	void resizeGL(int, int) override;
	void paintGL() override;
	void keyPressEvent(QKeyEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void wheelEvent(QWheelEvent*) override;

protected slots:
	void onTimeout();

private:
	void compileShaderProgram(QOpenGLShaderProgram&, const QString&, const QString&);
	void loadCube();
	void drawCube();
	void zoomIn(int);

private:
	float zoom{ 60.0f };
	float fps{ 60.0f };
	int mouse_x{ 0 }, mouse_y{ 0 };

	Camera camera;
	QMatrix4x4 projection;
	QTimer* timer;

private:
	QOpenGLShaderProgram cubeShaderProgram;
	GLuint cubeVAO{ 0 }, cubeVBO{ 0 }, cubeEBO{ 0 }, cubeTexture{ 0 };
};
