#include "widget.h"

#include <QtWidgets/QApplication>

#include <iostream>

void hello()
{
    std::cout << "----------------------" << std::endl;
    std::cout << "| Particle Collision |" << std::endl;
    std::cout << "----------------------" << std::endl;
    std::cout << "* Author: Yihao Liu" << std::endl;
}

int main(int argc, char *argv[])
{
    hello();
    QApplication a(argc, argv);
    Widget w;
    w.show();
    int code = a.exec();
    std::cout << "----------------------" << std::endl;
    system("pause");
    return code;
}
