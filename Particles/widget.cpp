#include "widget.h"
#include "gui_handler.h"
#include "utils.h"

#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::WidgetClass())
{
    ui->setupUi(this);
    setWindowTitle("粒子碰撞");
    // 将GUI_HANDLER挂到窗口上
    auto handler = GUIHandler::Inst();
    connect(handler, SIGNAL(infoMsg(QString)), this, SLOT(onInfoMsgReceived(QString)));
    connect(handler, SIGNAL(warnMsg(QString)), this, SLOT(onWarnMsgReceived(QString)));
    connect(handler, SIGNAL(errMsg(QString)), this, SLOT(onErrMsgReceived(QString)));
    // 连接信号
    connect(ui->openGLWidget, SIGNAL(renderTimeChange(int)), this, SLOT(onRenderTimeChanged(int)));
    connect(ui->openGLWidget, SIGNAL(cameraChange(QVector3D, QVector3D)), this, SLOT(onCameraChanged(QVector3D, QVector3D)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onInfoMsgReceived(QString infoMsg)
{
    QMessageBox::information(this, tr("提示"), infoMsg);
}

void Widget::onWarnMsgReceived(QString warnMsg)
{
    QMessageBox::warning(this, tr("警告"), warnMsg);
}

void Widget::onErrMsgReceived(QString errMsg)
{
    QMessageBox::critical(this, tr("错误"), errMsg);
    exit(1);
}

void Widget::onRenderTimeChanged(int renderTime)
{
    ui->renderTimeLabel->setText(tr("render: ") + QString::number(renderTime) + tr("ms"));
}

void Widget::onCameraChanged(QVector3D eye, QVector3D center)
{
    QString camera = tr("Camera");
    camera += ": ";
    camera += vecToString(eye);
    camera += " -> ";
    camera += vecToString(center);
    ui->cameraLabel->setText(camera);
}
