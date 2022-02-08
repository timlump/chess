#include "mesh.h"
#include "scene.h"
#include "binding.h"
#include <string>
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace graphics
{
    int mesh_instance::create(lua_State* state)
    {
        std::string name = luaL_checkstring(state, 1);
        std::string filename = luaL_checkstring(state, 2);

        auto mesh_instance = std::make_shared<graphics::mesh_instance>(filename);
        s_mesh_instances[name] = mesh_instance;
        graphics::scene::get()->add_mesh(mesh_instance);
        return 0;
    }

    int mesh_instance::set_shader(lua_State* state)
    {
        std::string mesh_name = luaL_checkstring(state, 1);
        int layer = luaL_checkinteger(state, 2);
        std::string shader_name = luaL_checkstring(state, 3);
        
        s_mesh_instances[mesh_name]->m_shaders_layers[layer] 
            = graphics::shader::get_shader(shader_name);
        return 0;
    }

    int mesh_instance::set_position(lua_State* state)
    {
        std::string mesh_name = luaL_checkstring(state, 1);
        float x = luaL_checknumber(state, 2);
        float y = luaL_checknumber(state, 3);
        float z = luaL_checknumber(state, 4);

        s_mesh_instances[mesh_name]->m_position = {x,y,z};
        return 0;
    }

    void mesh_instance::register_lua_functions()
    {
        std::vector<luaL_Reg> funcs = {
            {"create",mesh_instance::create},
            {"set_shader",mesh_instance::set_shader},
            {"set_position",mesh_instance::set_position}
        };

        binding::lua::get()->bind("mesh", funcs);
    }

    mesh_instance::mesh_instance(std::string filename)
    {
        auto find_iter = s_mesh_data_cache.find(filename);
        if (find_iter != s_mesh_data_cache.end())
        {
            m_mesh = find_iter->second;
        }
        else
        {
            m_mesh = std::make_shared<graphics::vertex_mesh_data>(
                graphics::load_model(filename, glm::vec3(0.05)).vertices
            );
            s_mesh_data_cache[filename] = m_mesh;
        }
    }
    
    void mesh_instance::draw(int layer)
    {
        if (m_shaders_layers.empty()) {
            return;
        }

        auto shader = m_shaders_layers.find(layer);
        if (shader == m_shaders_layers.end()) {
            return;
        }

        if (m_mesh) {
            m_mesh->refresh(shader->second);

            if (on_begin_draw) {
                on_begin_draw();
            }

            auto shader_program = shader->second->m_shader_program;

            glm::mat4 model_matrix = glm::mat4(1.f);
            model_matrix = glm::scale(model_matrix, m_scale);
            
            model_matrix = glm::translate(model_matrix, m_position);

            model_matrix = glm::rotate(model_matrix, m_z_rotation, glm::vec3(0,0,1));
            model_matrix = glm::rotate(model_matrix, m_y_rotation, glm::vec3(0,1,0));
            model_matrix = glm::rotate(model_matrix, m_x_rotation, glm::vec3(1,0,0));

            GLint model_uniform = glGetUniformLocation(shader_program, "model");
            if (model_uniform != -1) {
                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
            }

            GLint view_uniform = glGetUniformLocation(shader_program, "view");
            if (view_uniform != -1) {
                glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(scene::get()->get_view_mat()));
            }
            
            GLint proj_uniform = glGetUniformLocation(shader_program, "project");
            if (proj_uniform != -1) {
                glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(scene::get()->get_proj_mat()));
            }

            GLint colour_location = glGetUniformLocation(shader_program, "colour");
            if (colour_location != -1) {
                glUniform3fv(colour_location, 1, glm::value_ptr(m_colour));
            }

            GLint view_pos_location = glGetUniformLocation(shader_program, "view_pos");
            if (view_pos_location != -1) {
                glUniform3fv(view_pos_location, 1, glm::value_ptr(scene::get()->m_camera_params.position));
            }

            GLint shadow_location = glGetUniformLocation(shader_program, "shadow_map");
            if (shadow_location != -1) {
                // should be texture unit 0
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, scene::get()->m_shadowmap_tex);
                glUniform1i(shadow_location, 0);
            }

            GLint light_location = glGetUniformLocation(shader_program, "light_pos");
            if (light_location != -1) {
                glm::vec3 light_pos = scene::get()->m_light_params.position;
                glUniform3fv(light_location, 1, glm::value_ptr(light_pos));
            }

            GLint light_mat_location = glGetUniformLocation(shader_program, "light_mat");
            if (light_mat_location != -1) {
                glUniformMatrix4fv(light_mat_location, 1, GL_FALSE, glm::value_ptr(scene::get()->m_light_space_mat));
            }

            m_mesh->draw();

            if (on_finish_draw) {
                on_finish_draw();
            }
        }
    }
}