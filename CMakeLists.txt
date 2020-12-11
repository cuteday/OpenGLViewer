cmake_minimum_required ( VERSION 3.1)       
project                ( OpenGLViewer )          

set(CMAKE_EXPORT_COMPILE_COMMANDS 1) 
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_definitions(-std=c++11)
set(CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0" )
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG " )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/build/lib)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)  
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


add_compile_options(
	-Wno-invalid-offsetof
	-Wno-c++11-extensions
)

add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories (
	/usr/local/include 
	${PROJECT_SOURCE_DIR}/include 
	${PROJECT_SOURCE_DIR}/src
	${ASSIMP_INCLUDE_DIRS})

set(SRC_LIST 
	${PROJECT_SOURCE_DIR}/include/glad.cpp
	${PROJECT_SOURCE_DIR}/include/stb_image.h 
	${PROJECT_SOURCE_DIR}/src/shader.hpp 
	${PROJECT_SOURCE_DIR}/src/mesh.hpp 
	${PROJECT_SOURCE_DIR}/src/model.hpp 
	${PROJECT_SOURCE_DIR}/src/filter.hpp 
	${PROJECT_SOURCE_DIR}/src/skybox.hpp 
	${PROJECT_SOURCE_DIR}/src/multisample.hpp )

set(TARGET 
	viewer)

foreach(executable IN LISTS TARGET)
	add_executable(${executable} ${PROJECT_SOURCE_DIR}/src/${executable}.cpp ${SRC_LIST})
	target_include_directories(${executable} PUBLIC ${OPENGL_INCLUDE_DIR})
	target_link_libraries(${executable} glfw)
	target_link_libraries(${executable} ${ASSIMP_LIBRARIES})
	target_link_libraries(${executable} ${OPENGL_gl_LIBRARY})
endforeach()