cmake_minimum_required(VERSION 3.1.0)

project(qwinff)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt5Network REQUIRED)

add_subdirectory(src)

install(FILES qwinff.desktop DESTINATION share/applications)
