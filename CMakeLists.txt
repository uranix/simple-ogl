cmake_minimum_required(VERSION 2.8)
project(opengl)

set( glew-cmake_BUILD_SHARED OFF CACHE BOOL "foo")
set( glew-cmake_BUILD_STATIC ON CACHE BOOL "bar")

set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "baz")
set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "qux")

set( FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "quux")

find_package(OpenGL REQUIRED)
find_package(Threads)

add_subdirectory(external/freeglut)
add_subdirectory(external/glew)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(external/freeglut/include)
include_directories(external/glew/include)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")
set(SOURCES main.cpp Engine.cpp Renderer.cpp EngineFacede.cpp tinyfiledialogs.c)

configure_file(shader.vert shader.vert COPYONLY)
configure_file(shader.frag shader.frag COPYONLY)

add_executable(main ${SOURCES})
target_link_libraries(main ${OPENGL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} freeglut_static libglew_static)
