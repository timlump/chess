#pragma once
#include "shader.h"

#include <memory>
#include <map>
#include <vector>
#include <functional>

#include "mesh_data.h"

namespace graphics
{
    class mesh_instance
    {
        public:
            mesh_instance(std::string filename);

            void draw(int layer = 0);
            
            std::function<void()> on_begin_draw = nullptr;
            std::function<void()> on_finish_draw = nullptr;

            glm::vec3 m_position = glm::vec3(0.f);
            glm::vec3 m_scale = glm::vec3(1.f);
            float m_x_rotation = 0;
            float m_y_rotation = 0;
            float m_z_rotation = 0;

            glm::vec3 m_colour = glm::vec3(1.f);

            std::map<int,std::shared_ptr<shader>> m_shaders_layers;

            static void register_lua_functions();

        private:
            std::shared_ptr<mesh_data> m_mesh = nullptr;

            inline static std::map<std::string, std::shared_ptr<mesh_data>> s_mesh_data_cache;
            inline static std::map<std::string, std::shared_ptr<mesh_instance>> s_mesh_instances;

            static int create(lua_State* state);
            static int set_shader(lua_State* state);
            static int set_position(lua_State* state);
    };
}