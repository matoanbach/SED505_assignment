cmake_minimum_required(VERSION 3.13)
project(AutoDrivingSystem)

set(CMAKE_CXX_STANDARD 23)

file(GLOB FILES AutoDrivingSystem.cpp)

add_executable(AutoDrivingSystem ${FILES})
