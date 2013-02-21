/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <QDir>
#include <QSettings>
#include "ui/mainwindow.h"
#include "services/paths.h"
#include "converter/ffmpeginterface.h"
#include "converter/mediaprobe.h"
#include "converter/exepath.h"
#include "services/notification.h"

/**
 * @brief Find the absolute path of the translation of the current locale.
 *
 * @return the absolute path of the translation or an empty string if file not found.
 * @note This function must be called after @c Paths has been initialized.
 */
static QString find_translation_file()
{
    QString locale = QLocale::system().name(); // language code + country code (xx_XX)
    QString language = locale.mid(0, 2); // language code (first two chars of locale)
    QString translation_file_basename =
            QDir(Paths::translationPath()).absoluteFilePath("qwinff_");

    // look for qwinff_xx_XX.qm in the translation directory
    QString translation_language_country = translation_file_basename + locale + ".qm";
    if (QFile(translation_language_country).exists())
        return translation_language_country;

    // look for qwinff_xx.qm in the translation directory
    QString translation_language = translation_file_basename + language + ".qm";
    if (QFile(translation_language).exists())
        return translation_language;

    // translation for current locale not found, return empty string
    return "";
}

int main(int argc, char *argv[])
{
    // Create Application.
    QApplication app(argc, argv);

    // Register QSettings information.
    app.setOrganizationName("qwinff");
#ifdef PORTABLE_APP // Portable App: Save settings in <exepath>/qwinff.ini.
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope
                       , app.applicationDirPath());
    qDebug() << "Setting path: " + app.applicationDirPath();
#else // Save settings in <home>/.qwinff/qwinff.ini
    if (true) {
        QString settings_dir = QDir(QDir::homePath()).absoluteFilePath(".qwinff");
        QDir().mkpath(settings_dir);
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settings_dir);
        qDebug() << "Setting path: " + settings_dir;
    }
#endif

    Paths::setAppPath(app.applicationDirPath());

#ifdef FFMPEG_IN_DATA_PATH // Search FFmpeg in <datapath>/ffmpeg/
    ExePath::setPath("ffmpeg", Paths::dataFileName("ffmpeg/ffmpeg"));
    ExePath::setPath("ffprobe", Paths::dataFileName("ffmpeg/ffprobe"));
    ExePath::setPath("sox", Paths::dataFileName("sox/sox"));
#else // Search FFmpeg in environment variables
    ExePath::setPath("ffmpeg", "ffmpeg");
    ExePath::setPath("ffprobe", "ffprobe");
    ExePath::setPath("sox", "sox");
#endif

    // Construct a string list containing all input filenames.
    QStringList inputFiles(app.arguments());
    inputFiles.removeFirst(); // Exclude self executable name.

    // Setup translation
    QTranslator translator;
    QString translation_filename = find_translation_file();
    if (!translation_filename.isEmpty()) {
        qDebug() << "Translation file: " + translation_filename;
        translator.load(translation_filename);
        app.installTranslator(&translator);
    }

    // Setup notification
    Notification::init();
    if (!Notification::setType(Notification::TYPE_LIBNOTIFY))
        Notification::setType(Notification::TYPE_NOTIFYSEND);

    // Create main window.
    MainWindow window(0, inputFiles);
    window.show();

    return app.exec();
}
