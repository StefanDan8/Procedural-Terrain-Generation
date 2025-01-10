# Group 44

Team members:

- Stefan Dan [stefan.dan@tum.de](mailto:stefan.dan@tum.de)
- Dominik Bereżański [ge63nem@mytum.de](mailto:ge63nem@mytum.de)
- Pawel Kedzia [pawel.kedzia@tum.de](mailto:pawel.kedzia@tum.de)

The project being 'Procedural Terrain Generation with Perlin Noise' described in https://gitlab.lrz.de/tum-i05/public/advprog-project-ideas/-/blob/master/procedural-terrain-generation/perlin-noise-procedural-terrain-generation.md?ref_type=heads

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
This creates the binaries `main` and `playground`.
`playground` corresponds to second sprint.
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
We are sorry, but we do not support MacOS.
The C++ code is indeed platform independent, but the `CMakeLists.txt` may need to be adjusted, as well as the build steps.
