cmake_minimum_required(VERSION 2.8)
project(meshview)

set( glew-cmake_BUILD_SHARED OFF CACHE BOOL "foo")
set( glew-cmake_BUILD_STATIC ON CACHE BOOL "bar")

set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "baz")
set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "qux")

set( FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "quux")

find_package(OpenGL REQUIRED)
find_package(Threads)

if(UNIX)
	list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")
else()
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -DFREEGLUT_LIB_PRAGMAS=0 -DGLEW_STATIC -DFREEGLUT_STATIC)
endif()

add_subdirectory(external/freeglut)
add_subdirectory(external/glew)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(external/freeglut/include)
include_directories(external/glew/include)

set(SOURCES main.cpp Engine.cpp Renderer.cpp EngineFacede.cpp Mesh.cpp DooSabin.cpp tinyfiledialogs.c)

configure_file(transform.vert transform.vert COPYONLY)
configure_file(triangles.geom triangles.geom COPYONLY)
configure_file(lines.geom lines.geom COPYONLY)
configure_file(light.frag light.frag COPYONLY)

add_executable(meshview ${SOURCES})
target_link_libraries(meshview ${OPENGL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} freeglut_static libglew_static)
