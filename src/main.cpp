#include <QtGui/QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <QDir>
#include "ui/mainwindow.h"
#include "services/paths.h"
#include "converter/ffmpeginterface.h"
#include "converter/mediaprobe.h"

int main(int argc, char *argv[])
{
    // Create Application.
    QApplication app(argc, argv);

    // Register QSettings information.
    app.setOrganizationName("qwinff");
    app.setApplicationName("qwinff");

    Paths::setAppPath(app.applicationDirPath());

#ifdef FFMPEG_IN_DATA_PATH // Search FFmpeg in <datapath>/ffmpeg/
    FFmpegInterface::setFFmpegExecutable(Paths::dataFileName("ffmpeg/ffmpeg"));
    MediaProbe::setFFprobeExecutable(Paths::dataFileName("ffmpeg/ffprobe"));
#else // Search FFmpeg in environment variables
    FFmpegInterface::setFFmpegExecutable("ffmpeg");
    MediaProbe::setFFprobeExecutable("ffprobe");
#endif

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
