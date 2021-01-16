
include_directories(${QT_INCLUDES} ../library/include)

set(elokab_fa_SRCS
    confirmdlg.cpp
    confirmdlg.h
    confirmdlg.ui
    filesactions.cpp
    filesactions.h
    main.cpp
    progressdlg.cpp
    progressdlg.h
    progressdlg.ui
)


add_executable(elokab-fa ${elokab_fa_SRCS})

set_target_properties(elokab-fa PROPERTIES OUTPUT_NAME elokab-fa)

set(ELOKAB_FA_LINK_LIBS
    Qt5::Core
    Qt5::Widgets

    elokablib
)

target_link_libraries(elokab-fa
PUBLIC
    ${ELOKAB_FA_LINK_LIBS}
)

install(TARGETS elokab-fa ${INSTALL_TARGETS_DEFAULT_ARGS})
