
include_directories(${QT_INCLUDES} src)

set(elokablib_SRCS
    menugen.cpp
    desktopfile.cpp
    edir.cpp
    eicon.cpp
    emimicon.cpp
    elokabsettings.cpp
    elokabapplication.cpp

    desktopfile.cpp
    desktopfile.h
    edir.cpp
    edir.h
    eicon.cpp
    eiconengin.cpp
    eiconengin.h
    eicon.h
    elokabapplication.cpp
    elokabapplication.h
    elokabsettings.cpp
    elokabsettings.h
    emimicon.cpp
    emimicon.h
    menugen.cpp
    menugen.h

)

add_library(elokablib STATIC ${elokablib_SRCS})

set_target_properties(elokablib PROPERTIES OUTPUT_NAME elokab)

set(ELOKABLIB_LINK_LIBS
    Qt5::Core
    Qt5::Widgets
)

target_link_libraries(elokablib
PUBLIC
    ${ELOKABLIB_LINK_LIBS}
)
