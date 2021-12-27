#pragma once

#include "shader.h"
#include "mesh.h"
#include <map>
#include <memory>

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
            static void create(int width, int height);
            static void destroy();
            static gfx* get();

            void draw(int layer = 0, bool render_to_texture = false);

            void add_mesh(std::shared_ptr<mesh_instance> instance, int order = render_order::standard);

            glm::mat4 m_view_mat = glm::mat4(1.f);
            glm::mat4 m_projection_mat = glm::mat4(1.f);
            glm::vec3 m_clear_colour = glm::vec3(0.39f, 0.58f, 0.93f);
            glm::vec3 m_light_pos = glm::vec3(10);

            GLuint m_render_tex;
            GLuint m_depth_stencil_rbo;

        private:
            static gfx* s_gfx;
            gfx(int width, int height);
            ~gfx();
            GLuint m_render_tex_framebuffer;
            std::multimap<int,std::shared_ptr<mesh_instance>> m_meshes;
    };
}