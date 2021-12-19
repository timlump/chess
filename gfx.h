#pragma once

#include "shader.h"
#include "mesh.h"

namespace graphics
{
    class gfx {
        public:
            gfx();

            ~gfx();

            void draw();

            void add_mesh(std::shared_ptr<mesh> mesh);

            void remove_mesh(std::shared_ptr<mesh> mesh);

            glm::mat4 m_view_mat = glm::mat4(1.f);
            glm::mat4 m_projection_mat = glm::mat4(1.f);
            glm::vec3 m_clear_colour = glm::vec3(0.39f, 0.58f, 0.93f);

        private:
            std::vector<std::shared_ptr<mesh>> m_meshes;
    };
}