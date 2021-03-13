include_directories(${QT_INCLUDES} ../library/include ../common)

set(ocfm_SRCS
    actions.cpp
    actions.h
    defines.h
    dialogactions.cpp
    dialogactions.h
    dialogaddaction.cpp
    dialogaddaction.h
    dialogrenamefiles.cpp
    dialogrenamefiles.h
    fileinformation.cpp
    fileinformation.h
    filterbar.cpp
    filterbar.h
    icons.qrc
    itemdelegate.cpp
    itemdelegate.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    messages.cpp
    messages.h
    myfilesystemmodel.cpp
    myfilesystemmodel.h
    mylistview.cpp
    mylistview.h
    mytreeview.cpp
    mytreeview.h
    owncloudcfg.cpp
    owncloudcfg.h
    owncloudsocket.cpp
    owncloudsocket.h
    pagewidget.cpp
    pagewidget.h
    pathwidget.cpp
    pathwidget.h
    placetree.cpp
    placetree.h
    searchview.cpp
    searchview.h
    settingsdlg.cpp
    settingsdlg.h
    settings.h
    tab.cpp
    tab.h
    thumbnails.cpp
    thumbnails.h
    trash.cpp
    trash.h
    udiskdevice.cpp
    udiskdevice.h

    dialogactions.ui
    dialogaddaction.ui
    dialogrenamefiles.ui
    fileinformation.ui
    mainwindow.ui
    pathwidget.ui
    progressdlg.ui
    searchview.ui
    settingsdlg.ui
)


add_executable(ocfm  ${ocfm_SRCS})

set_target_properties(ocfm PROPERTIES OUTPUT_NAME ocfm)

set(ELOKAB_FM_LINK_LIBS
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Network

    elokablib
    elocommonlib
)

target_link_libraries(ocfm
PUBLIC
    ${ELOKAB_FM_LINK_LIBS}
)

install(TARGETS ocfm ${INSTALL_TARGETS_DEFAULT_ARGS})
