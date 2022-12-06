#include "opengl_widget.h"
#include "gui_handler.h"

OpenGLWidget::OpenGLWidget(QWidget* parent)
	: QOpenGLWidget(parent), timer(new QTimer(this))
{
	grabKeyboard();
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

OpenGLWidget::~OpenGLWidget()
{
}

void OpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);
	
	projection.perspective(zoom, 1.0, 0.01, 100.0);
	compileShaderProgram(cubeShaderProgram, ":/cube.vert", ":/cube.frag");
}

void OpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
}

void OpenGLWidget::onTimeout()
{
}

void OpenGLWidget::compileShaderProgram(QOpenGLShaderProgram& shaderProgram, const QString& vert, const QString& frag)
{
	bool success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, vert);
	if (!success) {
		GUIHandler::Inst()->err("load " + vert + " failed!");
	}
	success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, frag);
	if (!success) {
		GUIHandler::Inst()->err("load " + frag + " failed!");
	}
	success = shaderProgram.link();
	if (!success) {
		GUIHandler::Inst()->err("link failed!");
	}
}
