CMAKE_MINIMUM_REQUIRED(VERSION 2.7)
PROJECT(say_lang)

QT5_ADD_TRANSLATION(QM_FILES 
	sayonara_lang_br.ts
	sayonara_lang_cs.ts
	sayonara_lang_de.ts
	sayonara_lang_en.ts
	sayonara_lang_fr.ts
	sayonara_lang_hu.ts
	sayonara_lang_it.ts
	sayonara_lang_mx.ts
	sayonara_lang_pl.ts
	sayonara_lang_pt.ts
	sayonara_lang_ro.ts
	sayonara_lang_ru.ts
	sayonara_lang_ua.ts
	sayonara_lang_zh_cn.ts
)

ADD_CUSTOM_TARGET(TRANSLATIONS_TARGET DEPENDS ${QM_FILES})
INSTALL(DIRECTORY ./icons DESTINATION share/sayonara/translations FILES_MATCHING
    PATTERN "*.png"
    PATTERN "*.svn" EXCLUDE
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${QM_FILES})
INSTALL(FILES ${QM_FILES} DESTINATION share/sayonara/translations)
