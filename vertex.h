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

        // limit to 4 bones per vertex
        int bone_ids[4] = {};
        float bone_weights[4] = {};

        void serialise(std::ofstream& out)
        {
            out.write((char*)glm::value_ptr(pos), sizeof(glm::vec3));
            out.write((char*)glm::value_ptr(normal), sizeof(glm::vec3));
            out.write((char*)glm::value_ptr(uv), sizeof(glm::vec2));
            out.write((char*)bone_ids, sizeof(bone_ids));
            out.write((char*)bone_weights, sizeof(bone_weights));
        }

        void deserialise(std::ifstream& in)
        {
            in.read((char*)glm::value_ptr(pos),sizeof(glm::vec3));
            in.read((char*)glm::value_ptr(normal),sizeof(glm::vec3));
            in.read((char*)glm::value_ptr(uv),sizeof(glm::vec2));
            in.read((char*)bone_ids, sizeof(bone_ids));
            in.read((char*)bone_weights, sizeof(bone_weights));
        }
    };

    struct bone
    {
        glm::mat4 offset_matrix;
        void serialise(std::ofstream& out)
        {
            out.write((char*)glm::value_ptr(offset_matrix), sizeof(glm::mat4));
        }

        void deserialise(std::ifstream& in)
        {
            in.read((char*)glm::value_ptr(offset_matrix), sizeof(glm::mat4));
        }
    };
}