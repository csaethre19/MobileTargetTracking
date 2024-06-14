cmake_minimum_required(VERSION 3.10)

project(MobileTargetTracking)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

include_directories(src/include)

add_executable(TestMain src/Main.cpp src/Tracking.cpp src/Camera.cpp src/UART.cpp src/VideoTransmitter.cpp)

TARGET_LINK_LIBRARIES(TestMain ${OpenCV_LIBS} raspicam raspicam_cv pthread)

add_executable(Temp src/webcam-demo.cpp src/Camera.cpp)

TARGET_LINK_LIBRARIES(Temp ${OpenCV_LIBS})

add_executable(CameraTest RaspicamTest.cpp)
target_link_libraries(CameraTest ${OpenCV_LIBS} raspicam raspicam_cv)

add_executable(Compare CompareFrames.cpp)
target_link_libraries(Compare ${OpenCV_LIBS} raspicam raspicam_cv)

add_executable(DebugVideo src/DebugVideo.cpp src/Camera.cpp)

TARGET_LINK_LIBRARIES(DebugVideo ${OpenCV_LIBS})

add_executable(Grad src/GradiantScreen.cpp)

TARGET_LINK_LIBRARIES(Grad ${OpenCV_LIBS})

add_executable(Red src/PaintScreen.cpp)

TARGET_LINK_LIBRARIES(Red ${OpenCV_LIBS})

# Testing Related:

#find_package(GTest CONFIG REQUIRED)

#add_executable(SimpleMathTests tests/SimpleMathTests.cpp)

#target_link_libraries(SimpleMathTests PRIVATE GTest::gtest GTest::gtest_main)

#enable_testing()

#add_test(NAME my_test COMMAND SimpleMathTests)
