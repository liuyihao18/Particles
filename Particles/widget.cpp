#include "widget.h"
#include "gui_handler.h"

#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetClass())
{
    ui->setupUi(this);
    auto handler = GUIHandler::Inst();
    connect(handler, SIGNAL(infoMsg(QString)), this, SLOT(onInfoMsgReceived(QString)));
    connect(handler, SIGNAL(warnMsg(QString)), this, SLOT(onWarnMsgReceived(QString)));
    connect(handler, SIGNAL(errMsg(QString)), this, SLOT(onErrMsgReceived(QString)));
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
