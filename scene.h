#pragma once

#include "shader.h"
#include "mesh.h"
#include <map>
#include <memory>

namespace graphics
{

    enum class render_type
    {
        standard,
        gbuffer,
        shadow_map
    };

    constexpr int SHADOW_WIDTH = 1024;
    constexpr int SHADOW_HEIGHT = 1024;

    class scene {
        public:
            static void destroy();
            static scene* get();

            void draw(render_type type, int layer = 0);

            void add_mesh(std::shared_ptr<mesh_instance> instance, int order = 0);

            glm::mat4 m_view_mat = glm::mat4(1.f);
            glm::mat4 m_projection_mat = glm::mat4(1.f);
            glm::vec3 m_clear_colour = glm::vec3(0.39f, 0.58f, 0.93f);

            GLuint m_colour_tex;
            GLuint m_normal_tex;
            GLuint m_position_tex;
            GLuint m_depth_stencil_rbo;
            
            GLuint m_shadowmap_tex;

        private:

            static scene* s_gfx;
            scene();
            ~scene();
            GLuint m_gbuffer_framebuffer;
            GLuint m_shadowmap_framebuffer;

            int m_width;
            int m_height;

            std::multimap<int,std::shared_ptr<mesh_instance>> m_meshes;
    };
}