cmake_minimum_required(VERSION 3.10)

project(OpenCVDemo)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(src/include)

add_executable(TestMain src/Main.cpp src/Tracking.cpp src/Camera.cpp)

TARGET_LINK_LIBRARIES(TestMain ${OpenCV_LIBS})

add_executable(Temp src/webcam-demo.cpp)

TARGET_LINK_LIBRARIES(Temp ${OpenCV_LIBS})