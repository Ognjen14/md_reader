cmake_minimum_required(VERSION 3.21)

project(MDReader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    enable_language(RC)
endif()
find_package(Qt6 6.10 REQUIRED COMPONENTS Core Gui Network Quick QuickControls2)

include(GNUInstallDirs)

if(WIN32)
    set(CMAKE_INSTALL_BINDIR ".")
endif()

qt_standard_project_setup(REQUIRES 6.10)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(importedcontent)
endif()

set(MDREADER_PLATFORM_SOURCES "")
if(WIN32)
    list(APPEND MDREADER_PLATFORM_SOURCES icons/mdreader.rc)
endif()

qt_add_executable(appMDReader
    src/main.cpp
    src/singleinstance.h src/singleinstance.cpp
    ${MDREADER_PLATFORM_SOURCES}
)

file(GLOB MDREADER_FONT_FILES CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts/*.ttf"
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts/*.otf"
)

set(MDREADER_QML_FILES
    qml/Main.qml
    qml/Theme.qml
    qml/ViewMode.qml
    qml/EditMode.qml
    qml/OutlinePane.qml
    qml/DocumentView.qml
    qml/TabStrip.qml
    qml/AppToolbar.qml
    qml/StatusStrip.qml
    qml/Banner.qml
    qml/FindBar.qml
    qml/RecentFilesMenu.qml
    qml/CheatSheet.qml
    qml/HelpSheet.qml
    qml/IconButton.qml
    qml/TextContextMenu.qml
    qml/FontPicker.qml
    qml/AppButton.qml
    qml/AppDialog.qml
    qml/SlimScrollBar.qml
    qml/SplitterHandle.qml
    qml/AutoScroller.qml
)

foreach(qml_file IN LISTS MDREADER_QML_FILES)
    get_filename_component(qml_base "${qml_file}" NAME)
    set_source_files_properties("${qml_file}" PROPERTIES QT_RESOURCE_ALIAS "${qml_base}")
endforeach()

set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(appMDReader
    URI MDReader
    VERSION 1.0
    QML_FILES ${MDREADER_QML_FILES}
    RESOURCES
        icons/mdreader.ico
        ${MDREADER_FONT_FILES}
    SOURCES
        src/appsettings.h src/appsettings.cpp
        src/codesyntax.h src/codesyntax.cpp
        src/documentoutline.h src/documentoutline.cpp
        src/draftstore.h src/draftstore.cpp
        src/fontcatalog.h src/fontcatalog.cpp
        src/fileio.h src/fileio.cpp
        src/markdowndocument.h src/markdowndocument.cpp
        src/markdownhighlighter.h src/markdownhighlighter.cpp
)

target_include_directories(appMDReader PRIVATE src)

target_compile_definitions(appMDReader PRIVATE
    MDREADER_VERSION="${PROJECT_VERSION}"
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(appMDReader PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Quick
    Qt6::QuickControls2
)

if(WIN32)
    target_link_libraries(appMDReader PRIVATE user32)
endif()

set_target_properties(appMDReader PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS appMDReader
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET appMDReader
    OUTPUT_SCRIPT MDREADER_DEPLOY_SCRIPT
    NO_UNSUPPORTED_PLATFORM_ERROR
    NO_TRANSLATIONS
)

if(WIN32)
    install(CODE "set(QT_DEPLOY_BIN_DIR \".\")")
endif()

install(SCRIPT ${MDREADER_DEPLOY_SCRIPT})

set(CPACK_PACKAGE_NAME "MDReader")
set(CPACK_PACKAGE_VENDOR "TopicDev")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Markdown viewer and editor")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "MDReader")
set(CPACK_PACKAGE_FILE_NAME "MDReader-${PROJECT_VERSION}-win64")

set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_EXECUTABLES "appMDReader" "MDReader")
set(CPACK_CREATE_DESKTOP_LINKS "appMDReader")

set(CPACK_NSIS_PACKAGE_NAME "MDReader")
set(CPACK_NSIS_DISPLAY_NAME "MDReader")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/mdreader.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/mdreader.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "appMDReader.exe")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "appMDReader.exe")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH OFF)

set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

set(CPACK_NSIS_DEFINES "RequestExecutionLevel user")
set(CPACK_NSIS_INSTALL_ROOT "$LOCALAPPDATA\\\\Programs")


function(mdreader_nsis_include out_var script)
    string(REPLACE "/" "\\\\" native "${CMAKE_CURRENT_SOURCE_DIR}/packaging/${script}")
    set(${out_var} "!include \\\"${native}\\\"" PARENT_SCOPE)
endfunction()

mdreader_nsis_include(CPACK_NSIS_EXTRA_INSTALL_COMMANDS install-associations.nsh)
mdreader_nsis_include(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS uninstall-associations.nsh)

include(CPack)
