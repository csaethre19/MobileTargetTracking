cmake_minimum_required(VERSION 3.10)

project(MobileTargetTracking)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Tracking src/tracking-demo.cpp)

TARGET_LINK_LIBRARIES(Tracking ${OpenCV_LIBS})
