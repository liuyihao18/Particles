#pragma once

#include "camera.h"

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>
#include <QtOpenGL/QOpenGLShaderProgram>
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

protected slots:
	void onTimeout();

private:
	void compileShaderProgram(QOpenGLShaderProgram&, const QString&, const QString&);

private:
	float zoom{ 60.0f };
	float fps{ 60.0f };

	Camera camera;
	QMatrix4x4 projection;
	QTimer* timer;

private:
	QOpenGLShaderProgram cubeShaderProgram;

};
