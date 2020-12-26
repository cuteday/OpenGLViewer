# OpenGL Viewer

An OpenGL tool reads 3D scenes and viewing.

This is an unfinished naive work, with nothing useful but for learning purpose ✧(≖ ◡ ≖✿)\
*Tested on OS X only*

## Render Pipeline (Currently)
1 Draw model and skybox (with optional HDR rendering) \
2 MSAA framebuffer \
3 Post-process framebuffer (with optional gamma-correction, user-defined image filter, tone mapping, blooming, etc.)

## Dependencies
[Assimp](https://github.com/assimp/assimp) \
[GLFW]() \
[GLAD](https://github.com/Dav1dde/glad) \
[GLM]()

## Usage
Install dependencies: assimp (GLFW and GLM already included in submodules)\
Replace the machine-dependent [GLAD file](https://glad.dav1d.de/) `glad.c` and `glad.h` in `/include/` 
~~~
git submodule --init 
mkdir build
cd build
cmake ..
make
~~~
*[TO BE UPDATED]*