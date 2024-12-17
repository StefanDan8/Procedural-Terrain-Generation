cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#project(group44 C CXX)
project(group44 CXX)

include(lib/gtest.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC -Wall -Wextra -fno-omit-frame-pointer -march=native -Wno-unknown-pragmas")

add_library(perlin src/PerlinAlgorithm.cpp src/PerlinOOP.cpp src/AppConfig.cpp)
target_include_directories(perlin PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

add_library(render src/Renderer.cpp)
target_include_directories(render PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
PRIVATE src)

add_library(gui src/UserInterface.cpp)
target_include_directories(gui PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
PRIVATE src)

add_library(lodepng include/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
PRIVATE include/lodepng)


### Tests
add_executable(test_all test/testSprint1.cpp)
target_link_libraries(test_all perlin gtest gtest_main pthread)

### Main
add_executable(main src/main.cpp)
target_link_libraries(main perlin render gui lodepng)

### Playground
add_executable(playground src/playground.cpp)
target_link_libraries(playground perlin render lodepng)

















