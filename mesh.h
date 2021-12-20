#pragma once
#include "shader.h"

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics
{
    struct vertex
    {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    std::vector<vertex> load_vertices_raw(std::string path);
    std::vector<vertex> load_vertices_obj(std::string path);

    class gfx;
    class mesh {
        public:
            mesh(std::shared_ptr<shader> shader, gfx* graphics, std::vector<vertex> vertices);

            ~mesh();

            void draw();

            // glm doesn't actually default matrices to identity
            glm::mat4 m_model_mat = glm::mat4(1.f);

        private:
            gfx* m_gfx;
            std::shared_ptr<shader> m_shader;
            int m_num_vertices;
            GLuint m_vbo;
            GLuint m_vao;
    };
}