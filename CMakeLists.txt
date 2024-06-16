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

# Example test:
add_executable(SimpleMathTests tests/SimpleMathTests.cpp)
target_link_libraries(SimpleMathTests PRIVATE GTest::gtest GTest::gtest_main)
add_test(NAME my_test COMMAND SimpleMathTests)

# MobileTargetTracking module tests:
add_executable(CameraTests tests/TestCamera.cpp src/Camera.cpp)
target_link_libraries(CameraTests PRIVATE GTest::gtest GTest::gtest_main ${OpenCV_LIBS})
add_test(NAME camera_tests COMMAND CameraTests)

add_executable(TrackingTests tests/TestTracking.cpp src/Tracking.cpp)
target_link_libraries(TrackingTests PRIVATE GTest::gtest GTest::gtest_main ${OpenCV_LIBS})
add_test(NAME tracking_tests COMMAND TrackingTests)

add_executable(UARTTests tests/TestUART.cpp src/UART.cpp)
target_link_libraries(UARTTests PRIVATE GTest::gtest GTest::gtest_main)
add_test(NAME uart_tests COMMAND UARTTests)

add_executable(VidTxTests tests/TestVideoTransmitter.cpp src/VideoTransmitter.cpp)
target_link_libraries(VidTxTests PRIVATE GTest::gtest GTest::gtest_main ${OpenCV_LIBS})
add_test(NAME vidtx_tests COMMAND VidTxTests)