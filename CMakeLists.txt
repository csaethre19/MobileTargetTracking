cmake_minimum_required(VERSION 3.10)

project(MobileTargetTracking)

set(OpenCV_DIR /opt)

set(raspicam_DIR /opt/raspicam/build)

FIND_PACKAGE(raspicam REQUIRED)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(src/include)

# TestMain
add_executable(TestMain src/Main.cpp src/Tracking.cpp src/Camera.cpp)

TARGET_LINK_LIBRARIES(TestMain ${OpenCV_LIBS})

# Webcam demo
add_executable(Temp src/webcam-demo.cpp)

TARGET_LINK_LIBRARIES(Temp ${OpenCV_LIBS})

# ImageDemo
add_executable(ImageDemo src/ImageDemo.cpp)

TARGET_LINK_LIBRARIES(ImageDemo ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(ImageDemo ${raspicam_LIBS})

# HelloWorld
add_executable(HelloWorld src/HelloWorld.cpp)

TARGET_LINK_LIBRARIES(HelloWorld ${OpenCV_LIBS})


