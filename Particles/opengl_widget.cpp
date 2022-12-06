#include "opengl_widget.h"
#include "constant.h"
#include "gui_handler.h"

#include <QKeyEvent>
#include <QMouseEvent>

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
    /* Initialize */
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    /* Perspective */
    projection.perspective(zoom, 1.0, 0.01, 100.0);

    /* Load */
    loadCube();

    /* Refresh */
    timer->start(1 / fps);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawCube();
}

void OpenGLWidget::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case Qt::Key_A:
        camera.translateLeft(0.2);
        break;
    case Qt::Key_D:
        camera.translateLeft(-0.2);
        break;
    case Qt::Key_W:
        camera.translateUp(0.2);
        break;
    case Qt::Key_S:
        camera.translateUp(-0.2);
        break;
    case Qt::Key_F:
        camera.translateForward(0.2);
        break;
    case Qt::Key_B:
        camera.translateForward(-0.2);
        break;
    case Qt::Key_Z:
        zoomIn(2);
        break;
    case Qt::Key_X:
        zoomIn(-2);
        break;
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent* e)
{
    mouse_x = e->pos().x();
    mouse_y = e->pos().y();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* e)
{
    int x = e->pos().x();
    int y = e->pos().y();

    if (abs(x - mouse_x) >= 3)
    {
        if (x > mouse_x)
        {
            camera.rotateLeft(-3.0);
        }
        else
        {
            camera.rotateLeft(3.0);
        }
        mouse_x = x;
    }

    if (abs(y - mouse_y) >= 3)
    {
        if (y > mouse_y)
        {
            camera.rotateUp(-3.0);
        }
        else
        {
            camera.rotateUp(3.0);
        }

        mouse_y = y;
    }
}

void OpenGLWidget::wheelEvent(QWheelEvent* e)
{
    zoomIn(2 * (e->angleDelta().y() / 120));
}

void OpenGLWidget::onTimeout()
{
    update();
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

void OpenGLWidget::loadCube()
{
    /* Shader */
    compileShaderProgram(cubeShaderProgram, CUBE_Vert_PATH, CUBE_FRAG_PATH);

    /* VAO, VBO and EBO */
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBE_INDICES), CUBE_INDICES, GL_STATIC_DRAW);
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // texture coord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Texture */
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    QImage img = QImage(":/textures/container.jpg").convertToFormat(QImage::Format_RGB888);
    if (img.isNull())
    {
        GUIHandler::Inst()->warn("load texture failed!");
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width(), img.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    cubeShaderProgram.bind();
    cubeShaderProgram.setUniformValue("texture1", 0);
    cubeShaderProgram.release();
}

void OpenGLWidget::drawCube()
{
    cubeShaderProgram.bind();
    {
        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // bind VAO
        glBindVertexArray(cubeVAO);
        QMatrix4x4 view = camera.getViewMatrix();
        cubeShaderProgram.setUniformValue("view", view);
        cubeShaderProgram.setUniformValue("projection", projection);
        QMatrix4x4 model;
        cubeShaderProgram.setUniformValue("model", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    cubeShaderProgram.release();
}

void OpenGLWidget::zoomIn(int z)
{
    projection = QMatrix4x4();
    zoom = zoom - z;
    zoom = fmin(zoom, 89);
    zoom = fmax(zoom, 1);
    projection.perspective(zoom, 1.0, 0.01, 100.0);
}
