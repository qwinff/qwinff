set(TS_FILES
  qwinff_ar.ts
  qwinff_cs_CZ.ts
  qwinff_de.ts
  qwinff_es_ES.ts
  qwinff_es_GT.ts
  qwinff_fr.ts
  qwinff_hu_HU.ts
  qwinff_it_IT.ts
  qwinff_ja_JP.ts
  qwinff_pl_PL.ts
  qwinff_pt_BR.ts
  qwinff_ro_RO.ts
  qwinff_ru.ts
  qwinff_tr_TR.ts
  qwinff_zh_CN.ts
  qwinff_zh_TW.ts
  )

qt5_create_translation(QM_FILES ${TS_FILES})
add_custom_target(translations ALL DEPENDS ${QM_FILES})

install(FILES ${QM_FILES} DESTINATION share/qwinff/translations)
