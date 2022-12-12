#pragma once

#include "ui_widget.h"

#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class WidgetClass;
};
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected slots:
    void onInfoMsgReceived(QString);
    void onWarnMsgReceived(QString);
    void onErrMsgReceived(QString);
    void onFpsChanged(int);
    void onCameraChanged(QVector3D, QVector3D);

private:
    Ui::WidgetClass *ui;
};
