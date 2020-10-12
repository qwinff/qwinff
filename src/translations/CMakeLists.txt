file(GLOB TS_FILES *.ts)
qt5_create_translation(QM_FILES ${TS_FILES})
add_custom_target(translations ALL DEPENDS ${QM_FILES})
