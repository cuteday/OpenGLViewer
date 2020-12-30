# OpenGL Viewer

An OpenGL tool reads 3D scenes and viewing.
This is an unfinished naive work, with nothing useful but for learning purpose ✧(≖ ◡ ≖✿)\
*Tested on OS X only*

## Features


## Render Pipeline (Currently)
1 Draw model and skybox (with optional HDR rendering) \
2 MSAA framebuffer \
3 Post-process framebuffer (with optional gamma-correction, user-defined image filter, tone mapping, blooming, etc.)

## Dependencies
OpenGL 4.1+ \
[Assimp](https://github.com/assimp/assimp) \
[GLFW]() \
[GLAD](https://github.com/Dav1dde/glad) \
[GLM]() \
[Freetype](https://www.freetype.org/download.html) \ 
[OpenCV]()

## Usage
Install dependencies: Assimp, Freetype (GLFW and GLM already included in submodules)\
Replace the machine-dependent [GLAD file](https://glad.dav1d.de/) `glad.c` and `glad.h` in `/include/` 
~~~
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
~~~
*[TO BE UPDATED]*