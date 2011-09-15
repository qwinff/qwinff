#include <QtGui/QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    // Create Application.
    QApplication app(argc, argv);

    // Register QSettings information.
    QCoreApplication::setOrganizationName("mediaformatconverter");
    QCoreApplication::setApplicationName("mediaformatconverter");

    // Create main window.
    MainWindow window;
    window.show();

    return app.exec();
}
