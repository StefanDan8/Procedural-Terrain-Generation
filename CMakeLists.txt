cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#project(group44 C CXX)
project(group44 C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

if(WIN32)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive- /Zc:__cplusplus")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
    else() # Assuming MinGW
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-omit-frame-pointer -march=native -Wno-unknown-pragmas")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -fno-omit-frame-pointer -march=native -Wno-unknown-pragmas")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()
#include(lib/gtest.cmake)

add_compile_definitions(SHADER_ROOT="${CMAKE_SOURCE_DIR}/shaders")
add_compile_definitions(OUTPUT_FOLDER_PATH="${CMAKE_SOURCE_DIR}/output")


### Graphics External
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/graphicsExternal)

add_subdirectory(graphicsExternal/glfw-3.4)
add_library(glad STATIC graphicsExternal/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/graphicsExternal/glad/include)

add_library(imgui graphicsExternal/imgui/src/imgui.cpp 
                graphicsExternal/imgui/src/imgui_tables.cpp 
                graphicsExternal/imgui/src/imgui_draw.cpp 
                graphicsExternal/imgui/src/imgui_widgets.cpp 
                graphicsExternal/imgui/src/backends/imgui_impl_glfw.cpp 
                graphicsExternal/imgui/src/backends/imgui_impl_opengl3.cpp
                )
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/graphicsExternal/imgui/include/
                                        ${CMAKE_CURRENT_SOURCE_DIR}/graphicsExternal/imgui/include/backends)


target_link_libraries(imgui glad glfw)

add_library(perlin src/PerlinUtils.cpp src/AppConfig.cpp)
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

add_library(shaders src/graphics/ShaderClass.cpp
                    src/graphics/EBO.cpp 
                    src/graphics/VAO.cpp 
                    src/graphics/VBO.cpp 
                    src/graphics/Camera.cpp
                    src/graphics/Mesh.cpp
                    src/graphics/ShaderManager.cpp
                    src/graphics/Camera2D.cpp
                    src/graphics/Camera3D.cpp)
target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/graphics)
target_link_libraries(shaders glad)

# ### Tests
# add_executable(test_all test/testSprint1.cpp)
# target_link_libraries(test_all perlin gtest gtest_main pthread)

# ### Main
# add_executable(main src/main.cpp)
# target_link_libraries(main perlin render gui lodepng)

### Playground
add_executable(playground src/playground.cpp src/Terrain3D.cpp src/Terrain.cpp src/PerlinLayer.cpp src/PerlinNoise.cpp src/Fuse.cpp)
target_include_directories(playground PRIVATE graphicsExternal.glfw-3.4/include graphicsExternal/glad/include graphicsExternal/imgui/include graphicsExternal/imgui/include/backends)
target_link_libraries(playground perlin render lodepng glfw glad imgui shaders)





















