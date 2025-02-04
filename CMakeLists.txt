# --- Reqirements ---
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(group44 C CXX)

# --- Options ---
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# --- Compiler flags ---
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

# --- Dependencies ---
add_compile_definitions(SHADER_ROOT="${CMAKE_SOURCE_DIR}/shaders")
add_compile_definitions(OUTPUT_FOLDER_PATH="${CMAKE_SOURCE_DIR}/output")
add_compile_definitions(GUI_IDENTIFIER="Controls")


# --- Graphics External
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

# --- Perlin Library
add_library(perlin src/PerlinUtils.cpp src/AppConfig.cpp)
target_include_directories(perlin PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# --- Shader Library
add_library(shader src/graphics/shader/ShaderClass.cpp 
                   src/graphics/shader/ShaderManager.cpp
            )
target_include_directories(shader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/graphics/shader)
target_link_libraries(shader glad)

# --- Window Library
add_library(window src/gui/Window.cpp)
target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/gui)
target_link_libraries(window imgui)

# --- Camera Library
add_library(camera src/graphics/camera/Camera.cpp 
                   src/graphics/camera/Camera2D.cpp 
                   src/graphics/camera/Camera3D.cpp
            )
target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/graphics/camera)
target_link_libraries(camera shader window)

# --- Lodepng Library
add_library(lodepng include/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
PUBLIC include/lodepng)

# --- Json Library
add_library(json INTERFACE)
target_include_directories(json INTERFACE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
${CMAKE_CURRENT_SOURCE_DIR}/include/json)

# --- Mesh Library
add_library(mesh src/graphics/mesh/VAO.cpp
                 src/graphics/mesh/EBO.cpp    
                 src/graphics/mesh/VBO.cpp 
                 src/graphics/mesh/Mesh.cpp
                )
target_include_directories(mesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/graphics/mesh)
target_link_libraries(mesh camera lodepng json)

# --- Terrain Library
add_library(terrain src/Terrain3D.cpp 
                    src/Terrain.cpp 
                    src/PerlinUtils.cpp
                    src/PerlinLayer.cpp 
                    src/PerlinNoise.cpp 
                    src/Fuse.cpp)
target_include_directories(terrain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(terrain mesh)

# --- GUI Library
add_library(gui src/gui/GUI.cpp)
target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/gui)
target_link_libraries(gui terrain)


# ----- Playground -----
# the main executable
add_executable(playground src/playground.cpp )
target_include_directories(playground PRIVATE graphicsExternal.glfw-3.4/include graphicsExternal/glad/include graphicsExternal/imgui/include graphicsExternal/imgui/include/backends)
target_link_libraries(playground perlin gui)





















