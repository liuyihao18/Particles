#pragma once

#include "camera.h"
#include "config.h"
#include "light.h"
#include "sphere.h"
#include "cube.h"
#include "material.h"
#include "system.h"

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QTimer>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent);
    ~OpenGLWidget();

protected:
    void initializeGL() override;
    void resizeGL(int, int) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

protected slots:
    void onTimeout();

private:
    // 初始化材质
    void initMaterial();
    // 编译着色器
    void compileShaderProgram(QOpenGLShaderProgram &, const QString &, const QString &);
    /* 加载物体 */
    void loadLight();  // 灯
    void loadCube();   // 容器
    void loadSphere(); // 粒子
    /* 绘制物体 */
    void drawLight();  // 灯
    void drawCube();   // 容器
    void drawSphere(); // 粒子
    // 缩放
    void zoomIn(int);

private:
    /* 显示 */
    float zoom{30.0f};          // 缩放
    float fps{FPS};             // 帧率
    int mouse_x{0}, mouse_y{0}; // 鼠标位置

    /* 场景 */
    Camera camera;         // 照相机
    Light light;           // 灯
    QMatrix4x4 projection; // 投影

    /* 引擎 */
    System system; // 系统
    QTimer *timer; // 定时器

private:
    QOpenGLShaderProgram sphereShaderProgram, cubeShaderProgram, lightShaderProgram;
    GLuint sphereVAO{0}, cubeVAO{0}, lightVAO{0};
    QVector<Material> materials;
};
