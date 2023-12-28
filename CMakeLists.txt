cmake_minimum_required(VERSION 3.21)
project(TetrisGame LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(REMOVE_RECURSE "build")
file(GLOB_RECURSE src *.cpp)

include_directories(./src)
add_executable(TetrisGame ${src})
target_link_libraries(TetrisGame Qt${QT_VERSION_MAJOR}::Widgets)
