#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MainWindow *window = new MainWindow;

    window->show();
    return application.exec();
}
