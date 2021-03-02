project(oCfm)

cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0063 NEW)

find_package(ECM REQUIRED NO_MODULE)
FIND_PACKAGE(Qt5LinguistTools)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)

# Uncomment to enable some tweaks for AppImage build
# add_definitions(-DBUILD_APPIMAGE=1)

# TODO: Build without deprecated QUrl methods
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(${QT_DEFINITIONS} )

remove_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

find_package(Qt5 CONFIG REQUIRED
    Core
    Gui
    Widgets
    Xml
    Concurrent
    Network

    LinguistTools
)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-override")
# disable the warning about null-pointer zero. FIXME.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")
#option(QTINDICATE_DISABLE "Disable support for notifications via indicator")
# disable the warning about null-pointer zero. FIXME.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-override")

add_definitions(${QT_DEFINITIONS} )

add_subdirectory(common)
add_subdirectory(library)
add_subdirectory(src)
add_subdirectory(elokab-fa)

# ===== Installs

install(FILES usr/share/applications/elokab-fm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} RENAME org.owncloud.elokab-fm.desktop)
INSTALL(DIRECTORY usr/share/elokab/elokab-fm/service DESTINATION ${DATA_INSTALL_DIR}/elokab-fm)
install(FILES usr/share/elokab/elokab-fm/system-file-manager.svg DESTINATION ${DATA_INSTALL_DIR}/elokab-fm)
# INSTALL(FILES etc/xdg/elokab/Archiver.conf DESTINATION ${SYSCONFDIR}/elokab/)
