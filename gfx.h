#pragma once

#include "shader.h"
#include "mesh.h"
#include <map>

namespace graphics
{
    enum render_order
    {
        standard = 0,
        reflector = 1,
        reflected = 2
    };

    class gfx {
        
        public:
            gfx();

            ~gfx();

            void draw();

            void add_mesh(std::shared_ptr<mesh> mesh, int order = render_order::standard);

            glm::mat4 m_view_mat = glm::mat4(1.f);
            glm::mat4 m_projection_mat = glm::mat4(1.f);
            glm::vec3 m_clear_colour = glm::vec3(0.39f, 0.58f, 0.93f);

        private:
            std::multimap<int,std::shared_ptr<mesh>> m_meshes;
    };
}