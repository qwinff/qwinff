cmake_minimum_required(VERSION 2.8.6)

project(qwinff)

find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtNetwork)
set(CMAKE_AUTOMOC TRUE)
include(${QT_USE_FILE})

add_subdirectory(src)
