#-------------------------------------------------
#
# Project created by QtCreator 2011-06-25T12:27:54
#
#-------------------------------------------------

QT       += core gui

TARGET = qwinff
TEMPLATE = app


SOURCES += main.cpp \
    ui/progressbar.cpp \
    ui/mainwindow.cpp \
    ui/convertlist.cpp \
    ui/conversionparameterdialog.cpp \
    ui/addtaskwizard.cpp \
    ui/aboutffmpegdialog.cpp \
    ui/optionsdialog.cpp \
    converter/presets.cpp \
    converter/mencoderinterface.cpp \
    converter/mediaprobe.cpp \
    converter/mediaconverter.cpp \
    converter/ffmpeginterface.cpp \
    converter/converterinterface.cpp \
    converter/conversionparameters.cpp \
    services/paths.cpp \
    services/extensions.cpp \
    services/filepathoperations.cpp \
    ui/aboutdialog.cpp

HEADERS  += \
    ui/progressbar.h \
    ui/mainwindow.h \
    ui/convertlist.h \
    ui/conversionparameterdialog.h \
    ui/addtaskwizard.h \
    ui/aboutffmpegdialog.h \
    ui/optionsdialog.h \
    converter/presets.h \
    converter/mencoderinterface.h \
    converter/mediaprobe.h \
    converter/mediaconverter.h \
    converter/ffmpeginterface.h \
    converter/converterinterface.h \
    converter/conversionparameters.h \
    services/paths.h \
    services/extensions.h \
    services/filepathoperations.h \
    ui/aboutdialog.h \
    version.h

FORMS    += \
    ui/conversionparameterdialog.ui \
    ui/addtaskwizard.ui \
    ui/mainwindow.ui \
    ui/aboutffmpegdialog.ui \
    ui/optionsdialog.ui \
    ui/aboutdialog.ui

RESOURCES += \
    images.qrc

TRANSLATIONS += \
    translations/qwinff_zh_TW.ts

unix {
    # If DATA_PATH is set, QWinFF searches data in DATA_PATH
    # Otherwise, it uses the executable path as data path.
    DEFINES += DATA_PATH=$(DATA_PATH)
}

win32 {
    # If FFMPEG_IN_DATA_PATH is set, QWinFF searches for FFmpeg executables
    # in <datapath>/ffmpeg/ instead of PATH.
    DEFINES += FFMPEG_IN_DATA_PATH
}

# This string is shown in the about box.
DEFINES += VERSION_ID_STRING=$(VERSION_ID_STRING)

# External Short Blocking Operation Timeout
DEFINES += OPERATION_TIMEOUT=30000
DEFINES += DEFAULT_THREAD_COUNT=4

OTHER_FILES += \
    settings.txt
