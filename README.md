# Lair engine

Lair is a simple game engine designed to make small, game-jam like games.

Lair is in a early stage of development and should be considered a prototype. It is written in C++11 with a binding in python to ease fast prototyping of games.


## Building

To build Lair, you will need
- CMake (http://www.cmake.org/)
- Eigen (http://eigen.tuxfamily.org)
- GLEW (http://glew.sourceforge.net/)
- SDL2 (https://www.libsdl.org/)
- SDL2_image (https://www.libsdl.org/projects/SDL_image/)
- SDL2_mixer (https://www.libsdl.org/projects/SDL_mixer/)
- Boost (hash, header only) (http://www.boost.org/)

To build the python binding (Probably compeltly broken now), you will need
- Python 3.3 (minimum) (https://www.python.org/), intepreter + library

To compile the tests, you will also need
- GTest (https://code.google.com/p/googletest/)


Compiling can be done with
```
mkdir some_build_dir && cd some_build_dir
cmake path_to_lair
make
```

You might need to tell CMake where to find the dependencies.

You can compile the demos with
```
make demo
```

And the tests with
```
make test
```
