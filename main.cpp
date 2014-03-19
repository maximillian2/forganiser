#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MainWindow *window = new MainWindow;
    QIcon *appIcon = new QIcon(":/images/fr.png");
    window->setWindowIcon(*appIcon);

    window->show();
    return application.exec();
}
