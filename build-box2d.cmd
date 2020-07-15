rem Use this batch file to build box2d for Visual Studio
cd external\box2d

rmdir /s /q build
mkdir build
cd build
cd
cmake .. -A "Win32"
cmake --build . --config DEBUG
rem cmake --build . --config RELEASE
rem start box2d.sln
