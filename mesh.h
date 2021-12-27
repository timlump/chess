#pragma once
#include "shader.h"

#include <memory>
#include <map>
#include <vector>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics
{
    struct vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    std::vector<vertex> load_vertices_obj(std::string path);

    class gfx;
    class mesh {
        public:
            mesh(std::vector<vertex> vertices);
            ~mesh();
            void draw();
            void refresh(std::shared_ptr<shader> shader);

        private:
            int m_num_vertices;
            GLuint m_vbo;
            GLuint m_vao;
    };

    class mesh_instance
    {
        public:
            mesh_instance();
            void add_shader(std::shared_ptr<shader> shader, int layer = 0);

            void scale(glm::vec3 value);
            void translate(glm::vec3 value);
            void rotate(float degrees, glm::vec3 axis);

            void draw(int layer = 0);
            
            std::function<void()> on_begin_draw = nullptr;
            std::function<void()> on_finish_draw = nullptr;

            // glm doesn't actually default matrices to identity
            glm::mat4 m_model_mat = glm::mat4(1.f);

            std::shared_ptr<mesh> m_mesh = nullptr;
        private:
            unsigned int m_id;
            int m_current_shader = 0;
            std::map<int,std::shared_ptr<shader>> m_shaders_layers;
    };
}