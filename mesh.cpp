#include "mesh.h"
#include "scene.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace graphics
{
    glm::vec2 to_vec2(std::string entry)
    {
        // string will look something like this
        // 
        // vt number1 number2
        //   ^       ^
        // first  second
        glm::vec2 result;
        auto first_space = entry.find(' ');
        auto second_space = entry.find(' ', first_space + 1);

        std::string x_str = entry.substr(first_space + 1, second_space - (first_space+1));
        std::string y_str = entry.substr(second_space + 1, entry.size() - second_space);

        result.x = std::stof(x_str);
        result.y = std::stof(y_str);

        return result;
    }

    glm::vec3 to_vec3(std::string entry)
    {
        // string will look something like this
        //
        // v number1 number2 number3
        //  ^       ^       ^
        // first  second  third
        glm::vec3 result = {};
        auto first_space = entry.find(' ');
        auto second_space = entry.find(' ', first_space + 1);
        auto third_space = entry.find(' ', second_space + 1);

        std::string x_str = entry.substr(first_space + 1, second_space - (first_space + 1));
        std::string y_str = entry.substr(second_space + 1, third_space - (second_space + 1));
        std::string z_str = entry.substr(third_space + 1, entry.size() - third_space);

        result.x = std::stof(x_str);
        result.y = std::stof(y_str);
        result.z = std::stof(z_str);

        return result;
    }

    struct face_entry {
        int vert_idx = {};
        int uv_idx = {};
        int normal_idx = {};
    };

    face_entry get_face(std::string entry)
    {
        // entry will look like number0/number1/number2
        face_entry result;
        auto first_slash = entry.find('/');
        auto second_slash = entry.find('/', first_slash + 1);

        std::string vert_idx_str = entry.substr(0,first_slash);
        std::string uv_idx_str = entry.substr(first_slash + 1, second_slash - (first_slash + 1));
        std::string normal_idx_str = entry.substr(second_slash + 1, entry.size() - second_slash);

        if (not vert_idx_str.empty()) {
            result.vert_idx = std::stoi(vert_idx_str) - 1;
        }

        if (not uv_idx_str.empty()) {
            result.uv_idx = std::stoi(uv_idx_str) - 1;
        }
        
        if (not normal_idx_str.empty()) {
            result.normal_idx = std::stoi(normal_idx_str) - 1;
        }
        
        return result;
    }

    std::vector<face_entry> get_faces(std::string entry)
    {
        std::vector<face_entry> result;

        auto first_space = entry.find(' ');
        auto second_space = entry.find(' ', first_space + 1);
        auto third_space = entry.find(' ', second_space + 1);

        std::string v0_str = entry.substr(first_space + 1, second_space - (first_space + 1));
        std::string v1_str = entry.substr(second_space + 1, third_space - (second_space + 1));
        std::string v2_str = entry.substr(third_space + 1, entry.size() - third_space);

        result.push_back(get_face(v0_str));
        result.push_back(get_face(v1_str));
        result.push_back(get_face(v2_str));

        return result;
    }

    std::vector<vertex> load_vertices_obj(std::string path, glm::vec3 scale, glm::vec3 offset)
    {
        std::vector<vertex> result;
        std::ifstream file(path);
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (file.is_open()) {
            while(not file.eof()) {
                std::string line = "";
                std::getline(file, line);
                if (line.size() > 2) {
                    switch(line[0]) {
                        case 'v':
                        {
                            switch(line[1]) {
                                case 't': {
                                    uvs.push_back(to_vec2(line));
                                } break;

                                case 'n': {
                                    normals.push_back(to_vec3(line));
                                } break;

                                default: {
                                    glm::vec3 vert = to_vec3(line);
                                    vert *= scale;
                                    vert += offset;
                                    vertices.push_back(vert);
                                } break;
                            }
                        } break;
                        
                        case 'f':
                        {
                            auto faces = get_faces(line);
                            for (auto& face : faces) {
                                vertex vert;
                                vert.pos = vertices[face.vert_idx];
                                if (not uvs.empty()) {
                                    vert.uv = uvs[face.uv_idx];
                                }
                                if (not normals.empty()) {
                                    vert.normal = normals[face.normal_idx];
                                }
                                result.push_back(vert);
                            }
                        } break;
                        default:
                            // not supported
                            break;
                    }
                }
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
                glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(scene::get()->m_view_mat));
            }
            
            GLint proj_uniform = glGetUniformLocation(shader_program, "project");
            if (proj_uniform != -1) {
                glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(scene::get()->m_projection_mat));
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


            GLint shadow_location = glGetUniformLocation(shader_program, "shadow_map");
            if (shadow_location != -1) {
                // should be texture unit 0
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, scene::get()->m_shadowmap_tex);
                glUniform1i(shadow_location, 0);
            }

            GLint light_location = glGetUniformLocation(shader_program, "light_pos");
            if (light_location != -1) {
                glUniform3fv(light_location, 1, glm::value_ptr(scene::get()->m_light_pos));
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