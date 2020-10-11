add_subdirectory(qmpwidget)

set(qwinff_source_files
  main.cpp

  converter/audiofilter.cpp
  converter/conversionparameters.cpp
  converter/converterinterface.cpp
  converter/exepath.cpp
  converter/ffmpeginterface.cpp
  converter/mediaconverter.cpp
  converter/mediaprobe.cpp
  converter/presets.cpp

  services/abstractpreviewer.cpp
  services/constants.cpp
  services/extensions.cpp
  services/ffplaypreviewer.cpp
  services/filepathoperations.cpp
  services/httpdownloader.cpp
  services/mplayerpreviewer.cpp
  services/notification.cpp
  services/notificationservice.cpp
  services/notificationservice-notifysend.cpp
  services/notificationservice-qt.cpp
  services/paths.cpp
  services/powermanagement-dummy.cpp
  services/settingtimer.cpp
  services/updatechecker.cpp
  services/updateinfoparser.cpp
  services/versioncompare.cpp
  services/xmllookuptable.cpp

  ui/aboutdialog.cpp
  ui/aboutffmpegdialog.cpp
  ui/addtaskwizard.cpp
  ui/conversionparameterdialog.cpp
  ui/convertlist.cpp
  ui/interactivecuttingdialog.cpp
  ui/mainwindow.cpp
  ui/mediaplayerwidget.cpp
  ui/myqmpwidget.cpp
  ui/optionsdialog.cpp
  ui/poweroffdialog.cpp
  ui/previewdialog.cpp
  ui/progressbar.cpp
  ui/rangeselector.cpp
  ui/rangewidgetbinder.cpp
  ui/timerangeedit.cpp
  ui/updatedialog.cpp
  )

set(qwinff_ui_files
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

set(qwinff_rc_files
  images.qrc
  )

add_definitions(
  -DDEFAULT_THREAD_COUNT=1
  -DOPERATION_TIMEOUT=30000
  )

add_executable(qwinff ${qwinff_source_files} ${qwinff_ui_files} ${qwinff_rc_files})
target_link_libraries(qwinff qmpwidget Qt5::Widgets Qt5::Network)
target_include_directories(qwinff PRIVATE .)
