#include "widget.h"
#include <cuda_runtime.h>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetClass())
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}
