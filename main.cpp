#include <QtGui/QApplication>
#include <QDebug>
#include "ui/mainwindow.h"
#include "ui/paths.h"
#include "converter/ffmpeginterface.h"

int main(int argc, char *argv[])
{
    // Create Application.
    QApplication app(argc, argv);

    // Register QSettings information.
    app.setOrganizationName("mediaformatconverter");
    app.setApplicationName("mediaformatconverter");

    FFmpegInterface::setFFmpegExecutable("ffmpeg");
    Paths::setAppPath(app.applicationDirPath());

    // Construct a string list containing all input filenames.
    QStringList inputFiles(app.arguments());
    inputFiles.removeFirst(); // Exclude self executable name.

    // Create main window.
    MainWindow window(0, inputFiles);
    window.show();

    return app.exec();
}
