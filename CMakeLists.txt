cmake_minimum_required(VERSION 3.10)

project(OpenCVDemo)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(src/include)

include_directories(include)

add_executable(TestMain src/Main.cpp)

target_include_directories(TestMain PRIVATE include)

TARGET_LINK_LIBRARIES(TestMain ${OpenCV_LIBS})
