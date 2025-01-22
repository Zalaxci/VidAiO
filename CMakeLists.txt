cmake_minimum_required(VERSION 3.15)

project(VidAiO)

find_package(Qt6 COMPONENTS Core Quick WebView Network REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)
qt_add_executable(VidAiO main.cpp web/web.h web/web.a)
qt_add_qml_module(VidAiO URI ga.calyx.VidAiO QML_FILES Main.qml)

target_include_directories(VidAiO PRIVATE web)
target_sources(VidAiO PRIVATE main.cpp web/web.h web/web.a)
target_link_libraries(VidAiO PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/web/web.a)
target_link_libraries(VidAiO PRIVATE Qt6::Core Qt6::Quick Qt6::WebView Qt6::Network)

install(TARGETS VidAiO DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
