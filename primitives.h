#pragma once
#include "mesh.h"
namespace primitives
{
    const std::vector<graphics::vertex> SQUARE = {
        {glm::vec3(-0.5f, 0.5f, 0.f), glm::vec2(0,0)},
        {glm::vec3(0.5f, 0.5f, 0.f),  glm::vec2(1,0)},
        {glm::vec3(0.5f, -0.5f, 0.f), glm::vec2(1,1)},
        {glm::vec3(0.5f, -0.5f, 0.f), glm::vec2(1,1)},
        {glm::vec3(-0.5f, -0.5f, 0.f),glm::vec2(0,1)},
        {glm::vec3(-0.5f, 0.5f, 0.f), glm::vec2(0,0)}
    };
}