cmake_minimum_required(VERSION 3.16)
project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Test)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    clientmanager.cpp
    clientmanager.h
    authwindow.h
    authwindow.cpp
    authwindow.ui
    adminwindow.h
    adminwindow.cpp
    adminwindow.ui
    crypto.h
    managechatdialog.h
    managechatdialog.cpp
    finduserdialog.h
    finduserdialog.cpp
)

add_executable(client ${PROJECT_SOURCES}
    thememanager.h
    thememanager.cpp)
target_link_libraries(client PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

add_executable(test_client test_crypto.cpp)
target_link_libraries(test_client PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(client)
endif()

if(MSVC)
    add_compile_options(/utf-8)
endif()
