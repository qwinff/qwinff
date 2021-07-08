/*  QWinFF - a qt4 gui frontend for ffmpeg
 *  Copyright (C) 2011-2013 Timothy Lin <lzh9102@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <QDir>
#include <QSettings>
#include <QMessageBox>
#include "ui/mainwindow.h"
#include "services/paths.h"
#include "converter/ffmpeginterface.h"
#include "converter/mediaprobe.h"
#include "converter/exepath.h"
#include "services/notification.h"
#include "services/constants.h"

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

/**
 * @brief Load program constants from constants.xml.
 * @return true on success, false on failure
 */
static bool load_constants()
{
#ifdef DATA_PATH
    QString app_path = QString(DATA_PATH);
#else
    QString app_path = qApp()->applicationDirPath();
#endif
    QString constant_xml_filename = QDir(app_path).absoluteFilePath("constants.xml");

    // open constant xml file
    QFile constant_xml(constant_xml_filename);
    constant_xml.open(QIODevice::ReadOnly);
    if (!constant_xml.isOpen()) {
        qCritical() << "Failed to read file: " << constant_xml_filename;
        QMessageBox::critical(0, "QWinFF",
                              QString("Cannot load %1. The program will exit now.")
                              .arg(constant_xml_filename));
        return false;
    }

    qDebug() << "Reading file: " << constant_xml_filename;
    // parse the xml file
    if (!Constants::readFile(constant_xml)) {
        QMessageBox::critical(0, "QWinFF",
                              QString("%1 contains error(s). "
                                      "Reinstall the application may solve the problem.")
                              .arg(constant_xml_filename));
        return false;
    }

    return true;
}

// register an external tool for use
// returns whether the tool can be successfully invoked
static bool register_tool(const char *id, const char *name)
{
    QString exefile = name; // default: use the program in PATH
#ifdef TOOLS_IN_DATA_PATH // Search external tools in <datapath>/tools
#ifdef Q_OS_WIN32 // executable files must end with .exe on MS Windows
    exefile = Paths::dataFileName("tools/%1.exe").arg(name);
#else
    exefile = Paths::dataFileName("tools/%1").arg(name);
#endif // Q_OS_WIN32
#endif // TOOLS_IN_DATA_PATH
    ExePath::setPath(id, exefile);
    if (ExePath::checkProgramAvailability(id))
        return true;
    // failed to invoke the program
    ExePath::setPath(id, ""); // unset the tool
    return false;
}

static bool register_tool(const char *name)
{
    return register_tool(name, name);
}

static void register_external_tools()
{
    // load user settings for the tools
    ExePath::loadSettings();
    // If the setting of ffmpeg is not available, register it again.
    // If "ffmpeg" doesn't exist on the system, try "avconv" instead.
    ExePath::checkProgramAvailability("ffmpeg")
            || register_tool("ffmpeg")
            || register_tool("ffmpeg", "avconv");
    // same as "ffmpeg" (try "avprobe" if "ffprobe" not available)
    ExePath::checkProgramAvailability("ffprobe")
            || register_tool("ffprobe")
            || register_tool("ffprobe", "avprobe");
    // same as above
    ExePath::checkProgramAvailability("ffplay")
            || register_tool("ffplay")
            || register_tool("ffplay", "avplay");
    // these tools have no alternative names
    register_tool("sox");
    register_tool("mplayer");
}

int main(int argc, char *argv[])
{
    // Create Application.
    QApplication app(argc, argv);

    if (!load_constants()) {
        app.exec();
        return EXIT_FAILURE;
    }

    // Register QSettings information.
    app.setOrganizationName("qwinff");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    if (Constants::getBool("Portable")) {
        // Portable App: Save settings in <exepath>/qwinff.ini.
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope
                           , app.applicationDirPath());
    } else {
        // Save settings to the default Qt location
    }
    qDebug() << "Settings filename: " + QSettings().fileName();

    Paths::setAppPath(app.applicationDirPath());

    register_external_tools();

    // Construct a string list containing all input filenames.
    QStringList inputFiles(app.arguments());
    inputFiles.removeFirst(); // Exclude self executable name.

    // Setup translation
    QTranslator translator;
    QString translation_filename = find_translation_file();
    if (!translation_filename.isEmpty()) {
        qDebug() << "Translation file: " << translation_filename;
        translator.load(translation_filename);
        app.installTranslator(&translator);
    }

    // Load translation for Qt library
    QTranslator translator_qt;
    translator_qt.load("qt_" + QLocale::system().name(), Paths::qtTranslationPath());
    app.installTranslator(&translator_qt);

    // Setup notification
    Notification::init();
    if (!Notification::setType(Notification::TYPE_LIBNOTIFY))
        Notification::setType(Notification::TYPE_NOTIFYSEND);

    // Create main window.
    MainWindow window(0, inputFiles);
    window.show();

    // Execute the main loop
    int status = app.exec();

    // Tear down notification
    Notification::release();

#ifndef TOOLS_IN_DATA_PATH
    // Save path settings
    ExePath::saveSettings();
#endif

    return status;
}
