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
- Install OpenCV on your computer with contrib modules included (this takes a long time)
    This is necessary to use the additional tracking algoirthms
    - git clone https://github.com/Microsoft/vcpkg.git
    - cd vcpkg
    - .\bootstrap-vcpkg.bat
    - .\vcpkg install opencv[contrib]:x64-windows
    - .\vcpkg integrate install

### When first building create the build folder:
mkdir build

### First time build:
cmake -B .\build\

###### First time building after installing EXTRA MODULES (path to vcpkg depends on where you cloned it!):
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake

### Build:
cmake --build .\build\ -- target <Executable_to_run>

### To Run:
.\build\Debug\<Executable>

### Running Tests:
Dependency: gtest \
./vcpkg install gtest \
Same steps for building/running above - specify test executable from tests directory
