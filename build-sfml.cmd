cmd external\sfml-2.5.1
rmdir /s /q build
mkdir build && cd build
cmake .. -A "Win32" -DBUILD_SHARED_LIBS=FALSE
cmake --build .
