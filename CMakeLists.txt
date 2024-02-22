cmake_minimum_required(VERSION 3.10)

project(MobileTargetTracking)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Tracking src/tracking-demo.cpp)

TARGET_LINK_LIBRARIES(Tracking ${OpenCV_LIBS})

add_executable(Temp src/webcam-demo.cpp)

TARGET_LINK_LIBRARIES(Temp ${OpenCV_LIBS})

# Testing Related:

find_package(GTest CONFIG REQUIRED)

add_executable(SimpleMathTests tests/SimpleMathTests.cpp)

target_link_libraries(SimpleMathTests PRIVATE GTest::gtest GTest::gtest_main)

enable_testing()

add_test(NAME my_test COMMAND SimpleMathTests)