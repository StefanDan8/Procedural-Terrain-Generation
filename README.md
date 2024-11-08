# Group 44

Team members:

- Stefan Dan [stefan.dan@tum.de](mailto:stefan.dan@tum.de)
- 
- 

## Build

The project contains a configuration file, such that it can be easily built with CMake. This way, you can build the project in the
terminal or with the IDE of your choice.

Building from Terminal: Start in the project directory.

```
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
```
This creates the binaries 'test_all' and 'main'.

TODO: End user does only need main. 'test_all' is only for development. Adjust later.
