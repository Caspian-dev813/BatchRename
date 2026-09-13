cmake_minimum_required(VERSION 3.19)
project(BatchRename LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "/Users/mikehuang/Qt/6.11.1/macos")

find_package(Qt6 6.11.1 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(BatchRename
        WIN32 MACOSX_BUNDLE
        main.cpp
        widget.cpp
        widget.h
        widget.ui
        Info.plist.in
)

set_target_properties(BatchRename PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
)

target_link_libraries(BatchRename
        PRIVATE
        Qt6::Core
        Qt6::Widgets
)

install(TARGETS BatchRename
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
        TARGET BatchRename
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})