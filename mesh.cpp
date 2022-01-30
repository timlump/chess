#include "mesh.h"
#include "scene.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace graphics
{
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

    mesh::mesh(std::vector<vertex> vertices) {
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

    mesh::~mesh()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void mesh::draw()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
    }

    void mesh::refresh(std::shared_ptr<shader> shader)
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

    mesh_instance::mesh_instance()
    {
         // for use by an id buffer - start at 1 so black means no id
        static unsigned int current_id = 1;
        m_id = current_id;
        current_id++;
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

        m_current_shader = layer;

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