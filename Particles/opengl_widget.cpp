#include "opengl_widget.h"
#include "gui_handler.h"
#include "utils.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QTime>
#include <QInputDialog>

#include <iostream>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), timer(new QTimer(this))
{
    bool ok = false;
    int numParticles = QInputDialog::getInt(this, tr("请输入粒子数量"), tr("粒子数量"), 
        PARTICLE_NUM, 1, 10000, 1, &ok);
    if (!ok) {
        exit(0);
    }
    std::cout << "* Cube size: " << Cube::GetContainer()->size << std::endl;
    std::cout << "* Particle number: " << numParticles << std::endl;
    this->numParticles = numParticles;
    system.init(
        numParticles,
        make_float3(Cube::GetContainer()->position), 
        Cube::GetContainer()->size
    );
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
        3.0f * Cube::GetContainer()->size
    );
    emit cameraChange(camera.getEye(), camera.getCenter());

    /* Perspective */
    projection.perspective(zoom, 1.0, 0.01, 100.0);

    /* Load */
    loadLight();
    loadCube();
    loadSphere();

    /* Material */
    initMaterial();

    /* Refresh */
    timer->start(1);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
    static int cnt = 0;
    static int sum = 0;
    static QTime lastTime;
    // 清除屏幕
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 绘制灯
    drawLight();
    // 用线框模式绘制容器
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawCube();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // 绘制粒子
    drawSphere();
    // 计算帧率
    QTime time = QTime::currentTime();
    cnt++;
    sum += lastTime.msecsTo(time);
    if (cnt == 10) {
        fps = 1000.0 * cnt / sum;
        emit fpsChange(static_cast<int>(fps));
        cnt = 0;
        sum = 0;
    }
    lastTime = time;
}

void OpenGLWidget::keyPressEvent(QKeyEvent *e)
{
    // 键盘：控制移动、缩放、重启等
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
    case Qt::Key_R:
        system.restart();
        break;
    }

    emit cameraChange(camera.getEye(), camera.getCenter());
}

void OpenGLWidget::mousePressEvent(QMouseEvent *e)
{
    // 鼠标：控制旋转
    mouse_x = e->pos().x();
    mouse_y = e->pos().y();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    // 鼠标：控制旋转
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

    // 更改重力方向
    system.setUp(make_float3(camera.getUp()));
    emit cameraChange(camera.getEye(), camera.getCenter());
}

void OpenGLWidget::wheelEvent(QWheelEvent *e)
{
    // 鼠标：控制缩放
    zoomIn(2 * (e->angleDelta().y() / 120));
}

// 更新
void OpenGLWidget::onTimeout()
{
    system.updateParticles(TIMESTAMP_RATIO_FPS / fps);
    update();
}

// 初始化材质
void OpenGLWidget::initMaterial()
{
    for (int i = 0; i < numParticles; i++)
    {
        materials.push_back(Material::random());
    }
}

// 编译着色器
void OpenGLWidget::compileShaderProgram(QOpenGLShaderProgram &shaderProgram, const QString &vert, const QString &frag)
{
    bool success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, vert);
    if (!success)
    {
        GUIHandler::Inst()->err("load " + vert + " failed!");
    }
    success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, frag);
    if (!success)
    {
        GUIHandler::Inst()->err("load " + frag + " failed!");
    }
    success = shaderProgram.link();
    if (!success)
    {
        GUIHandler::Inst()->err("link failed!");
    }
}

// 加载灯
void OpenGLWidget::loadLight()
{
    /* Shader */
    compileShaderProgram(lightShaderProgram, Light::VERT_PATH, Light::FRAG_PATH);

    /* VAO, VBO and EBO */
    QVector<float> lightVertices;
    QVector<unsigned int> lightIndices;
    Light::GetVertices(lightVertices);
    Light::GetIndices(lightIndices);
    GLuint lightVBO{0}, lightEBO{0};
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, lightVertices.size() * sizeof(float), lightVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightIndices.size() * sizeof(float), lightIndices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Model */
    light = Light(
        QVector3D(0.0f, 0.55f, 0.0f),
        QVector3D(1.0f, 1.0f, 1.0f),
        0.2f,
        0.5f);
}

// 加载容器
void OpenGLWidget::loadCube()
{
    /* Shader */
    compileShaderProgram(cubeShaderProgram, Cube::VERT_PATH, Cube::FRAG_PATH);

    /* VAO, VBO and EBO */
    QVector<float> cubeVertices;
    QVector<unsigned int> cubeIndices;
    Cube::GetVertices(cubeVertices);
    Cube::GetIndices(cubeIndices);
    GLuint cubeVBO{0}, cubeEBO{0};
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// 加载粒子
void OpenGLWidget::loadSphere()
{
    /* Shader */
    compileShaderProgram(sphereShaderProgram, Sphere::VERT_PATH, Sphere::FRAG_PATH);

    /* VAO, VBO and EBO */
    QVector<float> sphereVertices;
    QVector<unsigned int> sphereIndices;
    Sphere::GetVertices(sphereVertices);
    Sphere::GetIndices(sphereIndices);
    GLuint sphereVBO{0}, sphereEBO{0};
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// 绘制灯
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

// 绘制容器
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

// 绘制粒子
void OpenGLWidget::drawSphere()
{
    sphereShaderProgram.bind();
    {
        // light
        sphereShaderProgram.setUniformValue("light.position", light.position);
        sphereShaderProgram.setUniformValue("viewPos", camera.getEye());
        sphereShaderProgram.setUniformValue("light.ambient", light.ambient());
        sphereShaderProgram.setUniformValue("light.diffuse", light.diffuse());
        sphereShaderProgram.setUniformValue("light.specular", light.specular());

        QMatrix4x4 view = camera.getViewMatrix();
        sphereShaderProgram.setUniformValue("view", view);
        sphereShaderProgram.setUniformValue("projection", projection);

        float *pos = system.getPos();
        uint *type = system.getType();
        for (int i = 0; i < numParticles; i++)
        {
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

// 缩放
void OpenGLWidget::zoomIn(int z)
{
    projection = QMatrix4x4();
    zoom = zoom - z;
    zoom = fmin(zoom, 89);
    zoom = fmax(zoom, 1);
    projection.perspective(zoom, 1.0, 0.01, 100.0);
}
