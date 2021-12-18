#pragma once

#include "shader.h"
#include "mesh.h"

namespace graphics
{
    class gfx {
        public:
            gfx(int width, int height);

            ~gfx();

            bool draw();

            void add_mesh(std::shared_ptr<mesh> mesh);

            void remove_mesh(std::shared_ptr<mesh> mesh);

            glm::mat4 m_view_mat = glm::mat4(1.f);
            glm::mat4 m_projection_mat = glm::mat4(1.f);

        private:
            GLFWwindow *m_window = nullptr;
            std::vector<std::shared_ptr<mesh>> m_meshes;
    };
}