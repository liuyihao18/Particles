#pragma once

#include <QObject>

class GUIHandler : public QObject
{
    Q_OBJECT

public:
    static GUIHandler *Inst()
    {
        if (!handler)
        {
            handler = new GUIHandler();
        }
        return handler;
    }

private:
    GUIHandler(QObject *parent = nullptr) : QObject(parent) {}
    static GUIHandler *handler;

signals:
    void infoMsg(QString);
    void warnMsg(QString);
    void errMsg(QString);

public:
    void info(const QString &msg);
    void warn(const QString &msg);
    void err(const QString &msg);
};
