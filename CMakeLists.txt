cmake_minimum_required(VERSION 3.10)

project(MobileTargetTracking)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(src/include)

add_executable(TestMain src/Main.cpp src/Tracking.cpp src/Camera.cpp src/UART.cpp src/VideoTransmitter.cpp)
TARGET_LINK_LIBRARIES(TestMain ${OpenCV_LIBS} pthread)

# Testing Related:

# Enable testing
enable_testing()

# Add GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(SimpleMathTests tests/SimpleMathTests.cpp)
target_link_libraries(SimpleMathTests PRIVATE GTest::gtest GTest::gtest_main)
add_test(NAME my_test COMMAND SimpleMathTests)
