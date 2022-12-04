#pragma once

#include <QtWidgets/QWidget>
#include "ui_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetClass; };
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::WidgetClass *ui;
};
