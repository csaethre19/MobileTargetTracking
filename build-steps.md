## Windows
### Pre-requisites:
- Desktop development with C++ installed for Visual Studio Community - compiler for C++
- CMake installed https://cmake.org/download/
- C++ extension in Visual Studio Code
- When setting up need to select "CMake Select a Kit"
    - Choose the one that matches you system (amd or x86) and make sure it includes x64
- vcpkg installed via following steps:
    - git clone https://github.com/Microsoft/vcpkg.git
    - cd vcpkg
    - .\bootstrap-vcpkg.bat
- Install OpenCV and gtest:
    - cd vcpkg (if not already there)
    - .\vcpkg install opencv[contrib]:x64-windows (note: takes a very long time!)
    - .\vcpkg integrate install
    - .\vcpkg install gtest

### First Time Build:
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake \
\
If you are having build issues, remove build folder, and re-run first time build command

### Regular Build (use after making changes to re-compile):
cmake --build .\build\ --target \<Executable>
\
--target is optional, if not specified it will build every executable listed in CMakeLists.txt

### To Run:
.\build\Debug\ \<Executable>

### Running Tests:
Dependency: gtest \
Same steps for building/running above - specify test executable from tests directory

## Raspi

### 1. When cloning for the first time create a build folder
mkdir build <br>
cd build <br>
### 2. Run Cmake
cmake ..

### 3. Make project
make <br>

### 4. Run executable
Run the executable from inside the build folder <br>
./\<Executable> <br>

