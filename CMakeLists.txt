cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)

include(CTest)
enable_testing()
set(SOURCES
    shader.cpp
    mesh.cpp
    gfx.cpp)

add_executable(chess main.cpp ${SOURCES})
target_link_libraries(chess -lGL -lEGL -lglfw -lGLEW)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
