#include "mesh.h"
#include "scene.h"
#include "binding.h"
#include <string>
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace graphics
{
    mesh_data::mesh_data(std::vector<vertex> vertices)
    {
        static int id = 0;
        m_id = id;
        id++;

        m_num_vertices = vertices.size();

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

        m_min_dims = glm::vec3(std::numeric_limits<float>::max());
        m_max_dims = glm::vec3(std::numeric_limits<float>::min());
        for (auto& vert : vertices) {
            for (int idx = 0 ; idx < 3 ; idx++) {
                m_min_dims[idx] = std::min(m_min_dims[idx], vert.pos[idx]);
                m_max_dims[idx] = std::max(m_max_dims[idx], vert.pos[idx]);
            }
        }
    }

    mesh_data::~mesh_data()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void mesh_data::draw()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
    }

    void mesh_data::refresh(std::shared_ptr<shader> shader)
    {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        shader->use();
        {
            GLint pos_attrib = glGetAttribLocation(shader->m_shader_program, "position");
            GLint uv_attrib = glGetAttribLocation(shader->m_shader_program, "uv");
            GLint normal_attrib = glGetAttribLocation(shader->m_shader_program, "normal");

            if (pos_attrib != -1) {
                glEnableVertexAttribArray(pos_attrib);
                glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
            }
            
            if (normal_attrib != -1) {
                glEnableVertexAttribArray(normal_attrib);
                glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                    (void*)sizeof(vertex::pos));
            }

            if (uv_attrib != -1) {
                glEnableVertexAttribArray(uv_attrib);
                glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                    (void*)(sizeof(vertex::pos) + sizeof(vertex::normal)));
            }
        }
    }

    int mesh_instance::create_mesh_instance(lua_State* state)
    {
        std::string name = luaL_checkstring(state, 1);
        std::string filename = luaL_checkstring(state, 2);

        auto mesh_instance = std::make_shared<graphics::mesh_instance>(filename);
        s_mesh_instances[name] = mesh_instance;
        graphics::scene::get()->add_mesh(mesh_instance);
        return 0;
    }

    int mesh_instance::set_shader_for_mesh_instance(lua_State* state)
    {
        std::string mesh_name = luaL_checkstring(state, 1);
        int layer = luaL_checkinteger(state, 2);
        std::string shader_name = luaL_checkstring(state, 3);
        
        s_mesh_instances[mesh_name]->m_shaders_layers[layer] 
            = graphics::shader::get_shader(shader_name);
        return 0;
    }

    void mesh_instance::register_lua_functions()
    {
        binding::lua::get()->bind("create_mesh_instance", mesh_instance::create_mesh_instance);
        binding::lua::get()->bind("set_shader_for_mesh_instance", mesh_instance::set_shader_for_mesh_instance);
    }

    model_data load_model(std::string path, glm::vec3 scale, glm::vec3 offset)
    {
        model_data result;
        std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
        if (file.is_open())
        {
            unsigned int num_vertices = 0;
            file.read((char*)&num_vertices, sizeof(num_vertices));

            for (int idx = 0 ; idx < num_vertices ; idx++) {
                vertex vert;
                
                vert.deserialise(file);
                vert.pos *= scale;
                vert.pos += offset;

                result.vertices.push_back(vert);
            }

            file.close();
        }
        return result;
    }

    mesh_instance::mesh_instance(std::string filename)
    {
         // for use by an id buffer - start at 1 so black means no id
        static int id = 0;
        m_id = id;
        id++;

        auto find_iter = s_mesh_data_cache.find(filename);
        if (find_iter != s_mesh_data_cache.end())
        {
            m_mesh = find_iter->second;
        }
        else
        {
            m_mesh = std::make_shared<graphics::mesh_data>(
                graphics::load_model(filename, glm::vec3(0.05)).vertices
            );
            s_mesh_data_cache[filename] = m_mesh;
        }
    }
    
    void mesh_instance::draw(int layer)
    {
        if (m_shaders_layers.empty()) {
            std::cerr << "Mesh has not shaders\n";
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

            GLint id_uniform = glGetUniformLocation(shader_program, "id_colour");
            if (id_uniform != -1) {
                glm::ivec3 id_colour = glm::vec3(0);
                id_colour[0] = 0xFF & m_id;
                id_colour[1] = 0xFF & (m_id << 8);
                id_colour[2] = 0xFF & (m_id << 16);
                glUniform3iv(id_uniform, 1, glm::value_ptr(id_colour));
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