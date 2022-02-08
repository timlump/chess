#pragma once

#include "vertex.h"
#include "shader.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics
{
    /*
        output bin format:
            num_vertices : unsigned int
                pos 
                normal
                uv
                bone id 0 - 3
                bone weight 0 - 3
                .
                .
                .
            num_bones : unsigned int
                parent_bone_idx
                offset_matrix
                .
                .
                .
        tbd
    */
    struct model_data
    {
        std::vector<vertex> vertices;
    };

    model_data load_model(std::string path,
        glm::vec3 scale = glm::vec3(1), glm::vec3 offset = glm::vec3(0.f));

    class mesh_data {
        public:
            virtual void draw() = 0;
            virtual void refresh(std::shared_ptr<shader> shader) {};

            glm::vec3 m_min_dims;
            glm::vec3 m_max_dims;
    };

    class vertex_mesh_data : public mesh_data{
        public:
            vertex_mesh_data(std::vector<vertex> vertices);
            ~vertex_mesh_data();
            void draw();
            void refresh(std::shared_ptr<shader> shader);

            

        private:
            int m_num_vertices;
            GLuint m_vbo;
            GLuint m_vao;
    };
}