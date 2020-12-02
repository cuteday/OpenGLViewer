cmake_minimum_required ( VERSION 3.0)       
project                ( OpenGLProgram )          

set(CMAKE_EXPORT_COMPILE_COMMANDS 1) 
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CXX_FLAGS -std=c++11)
set(CMAKE_CXX_FLAGS_DEBUG   "-O0" )
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG " )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/build/lib)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)  
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glfw)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

include_directories (/usr/local/include ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
set(SRC_LIST 
	${PROJECT_SOURCE_DIR}/include/glad.c 
	${PROJECT_SOURCE_DIR}/include/stb_image.h 
	${PROJECT_SOURCE_DIR}/src/shader.hpp 
	${PROJECT_SOURCE_DIR}/src/mesh.hpp 
	${PROJECT_SOURCE_DIR}/src/model.hpp 
	${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST})


target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})