#-------------------------------------------------
#
# Project created by QtCreator 2011-06-25T12:27:54
#
#-------------------------------------------------

QT       += core gui

TARGET = FormatConverter
TEMPLATE = app


SOURCES += main.cpp \
    ui/progressbar.cpp \
    ui/mainwindow.cpp \
    ui/convertlist.cpp \
    ui/conversionparameterdialog.cpp \
    converter/presets.cpp \
    converter/mencoderinterface.cpp \
    converter/mediaprobe.cpp \
    converter/mediaconverter.cpp \
    converter/ffmpeginterface.cpp \
    converter/converterinterface.cpp \
    converter/conversionparameters.cpp \
    ui/addtaskwizard.cpp \
    ui/aboutffmpegdialog.cpp \
    services/paths.cpp \
    services/extensions.cpp \
    services/filepathoperations.cpp \
    ui/optionsdialog.cpp

HEADERS  += \
    ui/progressbar.h \
    ui/mainwindow.h \
    ui/convertlist.h \
    ui/conversionparameterdialog.h \
    converter/presets.h \
    converter/mencoderinterface.h \
    converter/mediaprobe.h \
    converter/mediaconverter.h \
    converter/ffmpeginterface.h \
    converter/converterinterface.h \
    converter/conversionparameters.h \
    ui/addtaskwizard.h \
    ui/aboutffmpegdialog.h \
    services/paths.h \
    services/extensions.h \
    services/filepathoperations.h \
    ui/optionsdialog.h

FORMS    += \
    ui/conversionparameterdialog.ui \
    ui/addtaskwizard.ui \
    ui/mainwindow.ui \
    ui/aboutffmpegdialog.ui \
    ui/optionsdialog.ui

RESOURCES += \
    images.qrc

TRANSLATIONS += \
    translations/qwinff_zh_TW.ts \
    translations/qwinff_zh_CN.ts

unix {
    DEFINES += DATA_PATH=$(DATA_PATH)
}

# External Short Blocking Operation Timeout
DEFINES += OPERATION_TIMEOUT=30000

OTHER_FILES += \
    settings.txt
