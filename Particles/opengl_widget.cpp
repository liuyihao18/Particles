#include "opengl_widget.h"
#include "gui_handler.h"
#include "utils.h"

#include <QKeyEvent>
#include <QMouseEvent>

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent), timer(new QTimer(this)),
    system(PARTICLE_NUM, make_float3(Cube::GetContainer()->position), Cube::GetContainer()->size)
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

    /* Camera */
    camera = Camera(
        QVector3D(-0.25f, 0.28f, 0.33f),
        QVector3D(0, 1, 0),
        QVector3D(0, 0, 0),
        3.0f
    );

    /* Perspective */
    projection.perspective(zoom, 1.0, 0.01, 100.0);

    /* Load */
    loadSphere();
    loadCube();
    loadLight();

    /* Material */
    initMaterial();

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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawCube();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    case Qt::Key_C:
        qDebug() << "Camera:" << camera.getPosition();
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
    system.update(1 / fps);
    update();
}

void OpenGLWidget::initMaterial()
{
    for (int i = 0; i < PARTICLE_NUM; i++) {
        materials.push_back(Material::random());
    }
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

void OpenGLWidget::loadLight()
{
    /* Shader */
    compileShaderProgram(lightShaderProgram, Light::VERT_PATH, Light::FRAG_PATH);

    /* VAO, VBO and EBO */
    QVector<float> lightVertices;
    QVector<unsigned int> lightIndices;
    Light::GetVertices(lightVertices);
    Light::GetIndices(lightIndices);
    GLuint lightVBO{ 0 }, lightEBO{ 0 };
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, lightVertices.size() * sizeof(float), lightVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightIndices.size() * sizeof(float), lightIndices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Model */
    light = Light(
        QVector3D(0.0f, 0.55f, 0.0f),
        QVector3D(1.0f, 1.0f, 1.0f),
        0.2f,
        0.5f
    );
}

void OpenGLWidget::loadSphere()
{
    /* Shader */
    compileShaderProgram(sphereShaderProgram, Sphere::VERT_PATH, Sphere::FRAG_PATH);

    /* VAO, VBO and EBO */
    QVector<float> sphereVertices;
    QVector<unsigned int> sphereIndices;
    Sphere::GetVertices(sphereVertices);
    Sphere::GetIndices(sphereIndices);
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
}

void OpenGLWidget::loadCube()
{
    /* Shader */
    compileShaderProgram(cubeShaderProgram, Cube::VERT_PATH, Cube::FRAG_PATH);

    /* VAO, VBO and EBO */
    QVector<float> cubeVertices;
    QVector<unsigned int> cubeIndices;
    Cube::GetVertices(cubeVertices);
    Cube::GetIndices(cubeIndices);
    GLuint cubeVBO{ 0 }, cubeEBO{ 0 };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(float), cubeIndices.data(), GL_STATIC_DRAW);
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
}

void OpenGLWidget::drawCube()
{
    cubeShaderProgram.bind();
    {
        QMatrix4x4 view = camera.getViewMatrix();
        cubeShaderProgram.setUniformValue("view", view);
        cubeShaderProgram.setUniformValue("projection", projection);
        QMatrix4x4 model;
        model.translate(Cube::GetContainer()->position);
        model.scale(Cube::GetContainer()->size);
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
        model.translate(light.position);
        model.scale(light.size);
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
        sphereShaderProgram.setUniformValue("light.position", light.position);
        sphereShaderProgram.setUniformValue("viewPos", camera.getPosition());
        sphereShaderProgram.setUniformValue("light.ambient", light.ambient());
        sphereShaderProgram.setUniformValue("light.diffuse", light.diffuse());
        sphereShaderProgram.setUniformValue("light.specular", light.specular());

        QMatrix4x4 view = camera.getViewMatrix();
        sphereShaderProgram.setUniformValue("view", view);
        sphereShaderProgram.setUniformValue("projection", projection);

        float* pos = system.getPos();
        uint* type = system.getType();
        for (int i = 0; i < PARTICLE_NUM; i++) {
            QVector3D position(pos[3 * i], pos[3 * i + 1], pos[3 * i + 2]);
            // Material
            sphereShaderProgram.setUniformValue("material.ambient", materials[i].ambient);
            sphereShaderProgram.setUniformValue("material.diffuse", materials[i].diffuse);
            sphereShaderProgram.setUniformValue("material.specular", materials[i].specular);
            sphereShaderProgram.setUniformValue("material.shininess", materials[i].shininess);

            // Model
            QMatrix4x4 model;
            model.translate(position);
            model.scale(Sphere::GetProto(type[i])->radius);
            sphereShaderProgram.setUniformValue("model", model);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
        }
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
