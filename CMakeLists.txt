cmake_minimum_required(VERSION 3.7)
project(lastTask)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        avltree.h
        hash.h
        main.cpp
        memorymanager.h)

add_executable(lastTask ${SOURCE_FILES})