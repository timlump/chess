#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

namespace graphics
{
    struct vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;

        void serialise(std::ofstream& out)
        {
            out.write((char*)glm::value_ptr(pos), sizeof(glm::vec3));
            out.write((char*)glm::value_ptr(normal), sizeof(glm::vec3));
            out.write((char*)glm::value_ptr(uv), sizeof(glm::vec2));
        }

        void deserialise(std::ifstream& in)
        {
            in.read((char*)glm::value_ptr(pos),sizeof(glm::vec3));
            in.read((char*)glm::value_ptr(normal),sizeof(glm::vec3));
            in.read((char*)glm::value_ptr(uv),sizeof(glm::vec2));
        }
    };
}