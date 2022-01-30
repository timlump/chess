#pragma once
#include "shader.h"

#include <memory>
#include <map>
#include <vector>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vertex.h"

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

    class mesh {
        public:
            mesh(std::vector<vertex> vertices);
            ~mesh();
            void draw();
            void refresh(std::shared_ptr<shader> shader);

            glm::vec3 m_min_dims;
            glm::vec3 m_max_dims;

            int m_id;

        private:
            int m_num_vertices;
            GLuint m_vbo;
            GLuint m_vao;
    };

    class mesh_instance
    {
        public:
            mesh_instance();

            void draw(int layer = 0);
            
            std::function<void()> on_begin_draw = nullptr;
            std::function<void()> on_finish_draw = nullptr;

            glm::vec3 m_position = glm::vec3(0.f);
            glm::vec3 m_scale = glm::vec3(1.f);
            float m_x_rotation = 0;
            float m_y_rotation = 0;
            float m_z_rotation = 0;

            glm::vec3 m_colour = glm::vec3(1.f);

            std::shared_ptr<mesh> m_mesh = nullptr;

            std::map<int,std::shared_ptr<shader>> m_shaders_layers;

            int m_id;

        private:
            int m_current_shader = 0;
    };
}