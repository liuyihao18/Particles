#include "opengl_widget.h"
#include "constant.h"
#include "gui_handler.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QTime>

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
    loadSphere();
    loadCube();
    loadLight();

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
    drawSphere();
    drawCube();
    drawLight();
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

void OpenGLWidget::loadSphere()
{
    /* Shader */
    compileShaderProgram(sphereShaderProgram, SPHERE_VERT_PATH, SPHERE_FRAG_PATH);

    /* VAO, VBO and EBO */
    // Reference: https://blog.csdn.net/weixin_41234001/article/details/104701508
    QVector<float> sphereVertices;
    QVector<unsigned int> sphereIndices;
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
        }
    }
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }
    GLuint sphereVBO{ 0 }, sphereEBO{ 0 };
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    spherePos = { 1, 0, 0 };
}

void OpenGLWidget::loadCube()
{
    /* Shader */
    compileShaderProgram(cubeShaderProgram, CUBE_VERT_PATH, CUBE_FRAG_PATH);

    /* VAO, VBO and EBO */
    GLuint cubeVBO{ 0 }, cubeEBO{ 0 };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Position */
    cubePos = { 0, 0, 0 };
}

void OpenGLWidget::loadLight()
{
    /* Shader */
    compileShaderProgram(lightShaderProgram, LIGHT_VERT_PATH, LIGHT_FRAG_PATH);

    /* VAO, VBO and EBO */
    GLuint lightVBO{ 0 }, lightEBO{ 0 };
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Position */
    lightPos = { 1.0f, 1.2f, 0.0f };
}

void OpenGLWidget::drawCube()
{
    cubeShaderProgram.bind();
    {
        // light
        cubeShaderProgram.setUniformValue("light.position", lightPos);
        cubeShaderProgram.setUniformValue("viewPos", camera.getPosition());
        QVector3D lightColor;
        lightColor.setX(1.0f);
        lightColor.setY(1.0f);
        lightColor.setZ(1.0f);
        float diffuseDecay = 0.5f;
        float ambientDecay = 0.2f;
        cubeShaderProgram.setUniformValue("light.ambient", lightColor * ambientDecay);
        cubeShaderProgram.setUniformValue("light.diffuse", lightColor * diffuseDecay);
        cubeShaderProgram.setUniformValue("light.specular", lightColor);
        cubeShaderProgram.setUniformValue("material.ambient", 1.0f, 0.5f, 0.31f);
        cubeShaderProgram.setUniformValue("material.diffuse", 1.0f, 0.5f, 0.31f);
        cubeShaderProgram.setUniformValue("material.specular", 0.5f, 0.5f, 0.5f);
        cubeShaderProgram.setUniformValue("material.shininess", 32.0f);

        QMatrix4x4 view = camera.getViewMatrix();
        cubeShaderProgram.setUniformValue("view", view);
        cubeShaderProgram.setUniformValue("projection", projection);
        QMatrix4x4 model;
        model.translate(cubePos);
        cubeShaderProgram.setUniformValue("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    cubeShaderProgram.release();
}

void OpenGLWidget::drawLight()
{
    lightShaderProgram.bind();
    {
        QMatrix4x4 view = camera.getViewMatrix();
        lightShaderProgram.setUniformValue("view", view);
        lightShaderProgram.setUniformValue("projection", projection);
        QMatrix4x4 model;
        model.translate(lightPos);
        model.scale(0.2f);
        lightShaderProgram.setUniformValue("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    lightShaderProgram.release();
}

void OpenGLWidget::drawSphere()
{
    sphereShaderProgram.bind();
    {
        // light
        sphereShaderProgram.setUniformValue("light.position", lightPos);
        sphereShaderProgram.setUniformValue("viewPos", camera.getPosition());
        QVector3D lightColor;
        lightColor.setX(1.0f);
        lightColor.setY(1.0f);
        lightColor.setZ(1.0f);
        float diffuseDecay = 0.5f;
        float ambientDecay = 0.2f;
        sphereShaderProgram.setUniformValue("light.ambient", lightColor * ambientDecay);
        sphereShaderProgram.setUniformValue("light.diffuse", lightColor * diffuseDecay);
        sphereShaderProgram.setUniformValue("light.specular", lightColor);
        sphereShaderProgram.setUniformValue("material.ambient", 1.0f, 0.5f, 0.31f);
        sphereShaderProgram.setUniformValue("material.diffuse", 1.0f, 0.5f, 0.31f);
        sphereShaderProgram.setUniformValue("material.specular", 0.5f, 0.5f, 0.5f);
        sphereShaderProgram.setUniformValue("material.shininess", 32.0f);

        QMatrix4x4 view = camera.getViewMatrix();
        sphereShaderProgram.setUniformValue("view", view);
        sphereShaderProgram.setUniformValue("projection", projection);
        QMatrix4x4 model;
        model.translate(spherePos);
        model.scale(0.5);
        sphereShaderProgram.setUniformValue("model", model);

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
    }
    sphereShaderProgram.release();
}

void OpenGLWidget::zoomIn(int z)
{
    projection = QMatrix4x4();
    zoom = zoom - z;
    zoom = fmin(zoom, 89);
    zoom = fmax(zoom, 1);
    projection.perspective(zoom, 1.0, 0.01, 100.0);
}
