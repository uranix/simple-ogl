cmake_minimum_required(VERSION 2.8)
project(opengl)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")
set(SOURCES main.cpp Engine.cpp Renderer.cpp EngineFacede.cpp)

configure_file(shader.vert shader.vert COPYONLY)
configure_file(shader.frag shader.frag COPYONLY)

add_executable(main ${SOURCES})
target_link_libraries(main ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
