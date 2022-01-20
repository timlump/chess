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

    constexpr int SHADOW_WIDTH = 2048;
    constexpr int SHADOW_HEIGHT = 2048;

    class scene {
        public:
            static void destroy();
            static scene* get();

            void draw(render_type type, int layer = 0);

            void add_mesh(std::shared_ptr<mesh_instance> instance, int order = 0);

            glm::vec3 m_clear_colour = glm::vec3(0.39f, 0.58f, 0.93f);
            glm::mat4 m_light_space_mat = glm::mat4(1.f);

            GLuint m_colour_tex;
            GLuint m_normal_tex;
            GLuint m_position_tex;
            GLuint m_depth_stencil_rbo;
            
            GLuint m_shadowmap_tex;

            glm::vec3 m_up = glm::vec3(0,1,0);

            struct 
            {
                glm::vec3 position = glm::vec3(1.2f, 1.2f, 1.2f);
                glm::vec3 target = glm::vec3(0.0);
                glm::mat4 projection = glm::mat4(1.f);
            } m_camera_params;

            struct
            {
                glm::vec3 position = glm::vec3(1.26, 2.0, 1.05);
                glm::vec3 target = glm::vec3(0.0);
                glm::mat4 projection = glm::ortho(
                    -2.f, 2.0f, -2.f, 2.f, 1.f, 7.5f
                );
            } m_light_params;

            glm::mat4 get_view_mat() {
                return m_view_mat;
            }

            glm::mat4 get_proj_mat() {
                return m_proj_mat;
            }

        private:

            glm::mat4 m_view_mat = glm::mat4(1.f);
            glm::mat4 m_proj_mat = glm::mat4(1.f);

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