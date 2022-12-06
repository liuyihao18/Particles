#include "gui_handler.h"

GUIHandler *GUIHandler::handler = nullptr;

void GUIHandler::info(const QString &msg)
{
    emit infoMsg(msg);
}

void GUIHandler::warn(const QString &msg)
{
    emit warnMsg(msg);
}

void GUIHandler::err(const QString &msg)
{
    emit errMsg(msg);
}
