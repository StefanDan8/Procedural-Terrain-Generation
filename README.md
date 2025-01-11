# Group 44

Team members:

- Stefan Dan [stefan.dan@tum.de](mailto:stefan.dan@tum.de)
- Dominik Bereżański [ge63nem@mytum.de](mailto:ge63nem@mytum.de)
- Pawel Kedzia [pawel.kedzia@tum.de](mailto:pawel.kedzia@tum.de)

The project being 'Procedural Terrain Generation with Perlin Noise' described in https://gitlab.lrz.de/tum-i05/public/advprog-project-ideas/-/blob/master/procedural-terrain-generation/perlin-noise-procedural-terrain-generation.md?ref_type=heads

## Note for reviewers of Sprint 2

In this sprint we have added a graphical user interface (GUI) and rendering with OpenGL. Therefore, the project uses a handful of external libraries, which we have taken from the internet, without modifying any of them. These libraries are located in the `graphicsExternal` folder. They are

- `glad` https://glad.dav1d.de/
- `glfw-3.4` https://www.glfw.org/
- `glm` https://www.opengl.org/sdk/libs/GLM/
- `imgui` https://github.com/ocornut/imgui

To these libraries, we add `lodepng`, https://lodev.org/lodepng/, located in `include/lodepng`, which we have been using since Sprint 1.
We express our gratitude to the authors of these libraries for creating, maintaining and making them available to the public. 

All other files in this project are our own, with the mention that the architecture of the rendering engine follows the great OpenGL tutorial of Victor Gordan https://github.com/VictorGordan/opengl-tutorials (classes `EBO`, `VAO`, `VBO` are very close to the original, while classes `Camera` and `ShaderClass` have been substantially adjusted to fit our needs).

## Build

The project contains a configuration file, such that it can be easily built with CMake. This way, you can build the project in the
terminal or with the IDE of your choice.

Building from Terminal: Start in the project directory.

```
mkdir -p build/release
cd build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
```
This creates the binary `playground`, which corresponds to second sprint.
Run with 
```
./playground
```
from inside the release directory.

If you're using Windows as OS try instead (assuming you use MinGW for C++):

```
mkdir build\release
cd build\release
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..\..
mingw32-make
```

To run the program execute
```
start playground.exe
```

We are sorry, but we do not support MacOS.

The C++ code is indeed platform independent, but the `CMakeLists.txt` may need to be adjusted, as well as the build steps.

## Usage

The application has two operating modes: 2D and 3D.
The application starts in 2D mode and the user can switch at anytime between the two modes by clicking the corresponding button at the top of the GUI.
Depending on the resolution of the screen of the user, the GUI might be partially cropped at launch. This is a known issue which we will address in Sprint 3.
Using the program in full screen mode should fix any such issues.

![Example 2D](readmeImages/_2DWindow.png){ width=300px }
![Example 3D](readmeImages/_3DWindow.png){ width=300px }



