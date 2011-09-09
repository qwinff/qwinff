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
    converter/conversionparameters.cpp

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
    converter/conversionparameters.h

FORMS    += mainwindow.ui \
    ui/conversionparameterdialog.ui
