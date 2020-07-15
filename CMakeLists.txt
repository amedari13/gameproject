cmake_minimum_required(VERSION 3.8)

# https://cmake.org/cmake/help/latest/command/project.html
project(platform VERSION 0.0.1)

set( CMAKE_VERBOSE_MAKEFILE on )

add_executable(main 
    src/game.cpp
    src/level.cpp
    src/tinystr.cpp
    src/tinyxml.cpp
    src/tinyxmlerror.cpp
    src/tinyxmlparser.cpp)

target_compile_definitions(main PRIVATE
    -DSFML_STATIC)

target_include_directories(main PRIVATE 
    external/SFML-2.5.1/Include
    external/box2d/include)

target_link_directories(main PRIVATE
    debug external/SFML-2.5.1/build/lib/Debug
    debug external/box2d/build/src/Debug
    optimized external/SFML-2.5.1/build/lib/Release
    optimized external/box2d/build/src/Release)

target_link_libraries(main PUBLIC 
    box2d)

set_target_properties(main PROPERTIES
	CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
