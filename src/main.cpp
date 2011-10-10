#include <QtGui/QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <QDir>
#include "ui/mainwindow.h"
#include "services/paths.h"
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

    // Setup translation.
    QString locale = QLocale::system().name();
    QTranslator translator;
    QString translation_basename =
            QDir(Paths::translationPath()).absoluteFilePath("qwinff_");
    qDebug() << "Translation file: " + translation_basename + locale + ".qm";
    translator.load(translation_basename + locale);
    app.installTranslator(&translator);

    // Create main window.
    MainWindow window(0, inputFiles);
    window.show();

    return app.exec();
}
