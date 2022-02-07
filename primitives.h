#pragma once
#include "mesh.h"
namespace primitives
{
    const std::vector<graphics::vertex> SQUARE = {
        {glm::vec3(-0.5f, 0.f, 0.5f), glm::vec3(0.f,1.f,0.f), glm::vec2(0,0)},
        {glm::vec3(0.5f, 0.f, 0.5f),  glm::vec3(0.f,1.f,0.f), glm::vec2(1,0)},
        {glm::vec3(0.5f, 0.f, -0.5f), glm::vec3(0.f,1.f,0.f), glm::vec2(1,1)},
        {glm::vec3(0.5f, 0.f, -0.5f), glm::vec3(0.f,1.f,0.f), glm::vec2(1,1)},
        {glm::vec3(-0.5f, 0.f, -0.5f),glm::vec3(0.f,1.f,0.f), glm::vec2(0,1)},
        {glm::vec3(-0.5f, 0.f, 0.5f), glm::vec3(0.f,1.f,0.f), glm::vec2(0,0)}
    };

    /*
    (-1,1)                (1,1)
          __________________
          |        |        |
          |        |        |
          |________|________| 
          |        |        |
          |        |        |
          |________|________|
    (-1,-1)                (1,-1)
    */

    const std::vector<graphics::vertex> SQUARE2 = {
        {glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(0,1)},
        {glm::vec3(1.f, 1.f, 0.f),  glm::vec3(0.f,1.f,0.f), glm::vec2(1,1)},
        {glm::vec3(1.f, -1.f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(1,0)},
        {glm::vec3(1.f, -1.f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(1,0)},
        {glm::vec3(-1.f, -1.f, 0.f),glm::vec3(0.f,1.f,0.f), glm::vec2(0,0)},
        {glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(0,1)}
    };

    const std::vector<graphics::vertex> SQUARE3 = {
        {glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(0,0)},
        {glm::vec3(1.f, 1.f, 0.f),  glm::vec3(0.f,1.f,0.f), glm::vec2(1,0)},
        {glm::vec3(1.f, 0.5f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(1,1)},

        {glm::vec3(1.f, 0.5f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(1,1)},
        {glm::vec3(-1.f, 0.5f, 0.f),glm::vec3(0.f,1.f,0.f), glm::vec2(0,1)},
        {glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f,1.f,0.f), glm::vec2(0,0)}
    };
}