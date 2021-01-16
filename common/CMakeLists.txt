include_directories(${QT_INCLUDES} ../common ../library/include)

set(elocommonlib_SRCS
    about/widgetabout.cpp
    about/widgetabout.h
    about/widgetabout.ui

    filesutils/dialogfoldericons.cpp
    filesutils/dialogxdesktop.cpp
    filesutils/openwithdlg.cpp
    filesutils/propertiesdlg.cpp
    filesutils/symlinkdlg.cpp
    filesutils/dialogfoldericons.h
    filesutils/dialogxdesktop.h
    filesutils/openwithdlg.h
    filesutils/propertiesdlg.h
    filesutils/symlinkdlg.h
)

add_library(elocommonlib STATIC ${elocommonlib_SRCS})

set_target_properties(elocommonlib PROPERTIES OUTPUT_NAME elocommon)

set(COMMONLIB_LINK_LIBS
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
)

target_link_libraries(elocommonlib
PUBLIC
    ${COMMONLIB_LINK_LIBS}
)
