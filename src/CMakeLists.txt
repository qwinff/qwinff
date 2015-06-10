# options

set(DEFAULT_THREAD_COUNT 1 CACHE STRING
    "Default number of threads passed to ffmpeg (-threads N)"
    )
add_definitions(-DDEFAULT_THREAD_COUNT=${DEFAULT_THREAD_COUNT})

# find dependencies
find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtNetwork)
set(CMAKE_AUTOMOC TRUE)
include(${QT_USE_FILE})

# source files

set(qwinff_SOURCES
    main.cpp
    ui/progressbar.cpp
    ui/mainwindow.cpp
    ui/convertlist.cpp
    ui/conversionparameterdialog.cpp
    ui/addtaskwizard.cpp
    ui/aboutffmpegdialog.cpp
    ui/optionsdialog.cpp
    ui/aboutdialog.cpp
    ui/poweroffdialog.cpp
    ui/rangeselector.cpp
    ui/timerangeedit.cpp
    converter/presets.cpp
    converter/mediaprobe.cpp
    converter/mediaconverter.cpp
    converter/ffmpeginterface.cpp
    converter/converterinterface.cpp
    converter/conversionparameters.cpp
    services/paths.cpp
    services/extensions.cpp
    services/filepathoperations.cpp
    services/notification.cpp
    services/notificationservice.cpp
    services/notificationservice-qt.cpp
    services/notificationservice-notifysend.cpp
    services/powermanagement-dummy.cpp
    converter/audiofilter.cpp
    converter/exepath.cpp
    services/versioncompare.cpp
    services/updatechecker.cpp
    services/httpdownloader.cpp
    services/updateinfoparser.cpp
    services/constants.cpp
    services/xmllookuptable.cpp
    ui/updatedialog.cpp
    services/settingtimer.cpp
    services/ffplaypreviewer.cpp
    services/abstractpreviewer.cpp
    services/mplayerpreviewer.cpp
    ui/previewdialog.cpp
    ui/interactivecuttingdialog.cpp
    ui/myqmpwidget.cpp
    ui/mediaplayerwidget.cpp
    ui/rangewidgetbinder.cpp
    )

set(qwinff_FORMS
    ui/conversionparameterdialog.ui
    ui/addtaskwizard.ui
    ui/mainwindow.ui
    ui/aboutffmpegdialog.ui
    ui/optionsdialog.ui
    ui/aboutdialog.ui
    ui/poweroffdialog.ui
    ui/updatedialog.ui
    ui/previewdialog.ui
    ui/interactivecuttingdialog.ui
    ui/mediaplayerwidget.ui
    )

set(qwinff_TRANSLATIONS
    translations/qwinff_zh_TW.ts
    translations/qwinff_ja_JP.ts
    translations/qwinff_it_IT.ts
    translations/qwinff_cs_CZ.ts
    translations/qwinff_zh_CN.ts
    translations/qwinff_ru.ts
    translations/qwinff_es_ES.ts
    translations/qwinff_es_GT.ts
    translations/qwinff_ro_RO.ts
    translations/qwinff_de.ts
    translations/qwinff_tr_TR.ts
    translations/qwinff_ar.ts
    translations/qwinff_hu_HU.ts
    translations/qwinff_pl_PL.ts
)

# qmpwidget (embedded mplayer)
set(qwinff_SOURCES ${qwinff_SOURCES} qmpwidget/qmpwidget.cpp)

# resource files
qt4_add_resources(qwinff_RCC_SRCS images.qrc)

# .ui -> .cxx
qt4_wrap_ui(UI_SRCS ${qwinff_FORMS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(qwinff ${qwinff_SOURCES} ${UI_SRCS} ${qwinff_RCC_SRCS})
target_link_libraries(qwinff ${QT_LIBRARIES})
