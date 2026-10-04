cmake_minimum_required(VERSION 3.16)
project(BatchRename VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(BatchRename
        WIN32 MACOSX_BUNDLE
        main.cpp
        mainwindow.cpp
        file_scanner.cpp
        name_generator.cpp
        backup_manager.cpp
        rename_engine.cpp

        mainwindow.h
        models.h
        file_scanner.h
        name_generator.h
        backup_manager.h
        rename_engine.h

        mainwindow.ui

        Info.plist.in
)

set_target_properties(BatchRename PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.caspian.batchrename
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2026 Caspian. All rights reserved."
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
)

target_link_libraries(BatchRename PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

install(TARGETS BatchRename
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

qt_generate_deploy_app_script(
        TARGET BatchRename
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})