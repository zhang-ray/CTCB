cmake_minimum_required(VERSION 3.0.0)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(PROJECT_NAME ${FOLDER_NAME} CACHE STRING "Project Name")
set(CMAKE_CXX_STANDARD 14)

project(${PROJECT_NAME})

include_directories(.)

if(ANDROID_ABI)
    add_library(ctcb SHARED JNI.cpp)
endif(ANDROID_ABI)

add_executable(ctcb-test00 Test00.cpp)


