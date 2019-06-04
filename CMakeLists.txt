cmake_minimum_required(VERSION 3.0.0)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(PROJECT_NAME ${FOLDER_NAME} CACHE STRING "Project Name")
set(CMAKE_CXX_STANDARD 14)

project(${PROJECT_NAME})

include_directories(.)
file(GLOB_RECURSE FILES_SOURCES *.cpp *.cc *.c)
file(GLOB_RECURSE FILES_INCLUDE *.hpp *.hh *.h)
file(GLOB_RECURSE FILES_PROJECT CMakeLists.txt Makefile Android.mk)

add_executable(${PROJECT_NAME} ${FILES_INCLUDE} ${FILES_SOURCES} ${FILES_PROJECT})
