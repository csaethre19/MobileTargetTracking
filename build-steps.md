### Pre-requisites:
- Apparently need Desktop development with C++ installed for Visual Studio Community!!!
- CMake installed https://cmake.org/download/
- OpenCV installed https://opencv.org/releases/
- C++ extension in Visual Studio Code
- When setting up need to select "CMake Select a Kit"
    - Choose the one that matches you system (amd or x86) and make sure it includes x64
- The following environment variables added:
    - OpenCV_DIR : <opencv_build_folder_path>
- The following added to path environment variable:
    - opencv\build\x64\vc16\bin
    - opencv\build\x64\vc16\lib
    - C:\Program Files\CMake\bin
- CMakeLists.txt created in project folder


### When first building create the build folder:
mkdir build

###### First time building:
cmake -B .\build\

### Build:
cmake --build .\build\ -- target <Executable_to_run>

### To Run:
.\build\Debug\<Executable>

### Running Tests:
Same steps for building/running above - specify test executable from tests directory
