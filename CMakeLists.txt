cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache) # Less useful to do it for linking, see edit2
endif(CCACHE_FOUND)

set(SOURCES
    shader.cpp
    mesh.cpp
    scene.cpp
    compositor.cpp)

add_executable(chess main.cpp ${SOURCES})
target_link_libraries(chess -lGL -lEGL -lglfw -lGLEW)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_SOURCE_DIR}/meshes $<TARGET_FILE_DIR:${PROJECT_NAME}>/meshes)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_SOURCE_DIR}/scenes $<TARGET_FILE_DIR:${PROJECT_NAME}>/scenes)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
