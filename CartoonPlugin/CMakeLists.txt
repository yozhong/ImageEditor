cmake_minimum_required(VERSION 3.14)

project(CartoonPlugin LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    message(STATUS "OpenCV library status:")
    message(STATUS "    version: ${OpenCV_VERSION}")
    message(STATUS "    libraries: ${OpenCV_LIBS}")
    message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
endif()

include_directories(..)

add_library(CartoonPlugin SHARED
  cartoon_plugin.cpp
  cartoon_plugin.h
)

target_link_libraries(CartoonPlugin PRIVATE Qt${QT_VERSION_MAJOR}::Core ${OpenCV_LIBS})

target_compile_definitions(CartoonPlugin PRIVATE CARTOONPLUGIN_LIBRARY)
