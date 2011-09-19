#include <QtGui/QApplication>
#include "ui/mainwindow.h"
#include "ui/paths.h"

int main(int argc, char *argv[])
{

    Paths::setAppPath(QCoreApplication::applicationDirPath());

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
