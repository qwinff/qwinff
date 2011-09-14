#include <QtGui/QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // register QSettings information
    QCoreApplication::setOrganizationName("mediaformatconverter");
    QCoreApplication::setApplicationName("mediaformatconverter");

    return a.exec();
}
