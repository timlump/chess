#include "mesh.h"
#include "gfx.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace graphics
{
    std::vector<vertex> load_vertices_raw(std::string path)
    {
        std::vector<vertex> result;
        std::ifstream file(path);
        if (file.is_open()) {
            int count = 0;
            glm::vec3 pos = {};
            try {
                while(not file.eof()) {
                    std::string line = "";
                    std::getline(file, line, ' ');
                    pos[count] = std::stof(line);
                    count++;
                    if (count == 3) {
                        count = 0;
                        vertex v = {pos,glm::vec2(0)};
                        result.push_back(v);
                    }
                }
            }
            catch (std::invalid_argument& e) {
                // do nothing - its hit the end line
            }
            file.close();
        }
        else {
            throw std::runtime_error("unable to load vertices");
        }

        return result;
    }

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

    std::vector<vertex> load_vertices_obj(std::string path)
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
                                    vertices.push_back(to_vec3(line));
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

    mesh::mesh(std::shared_ptr<shader> shader, gfx* graphics, std::vector<vertex> vertices) {
        std::cout << "Mesh created\n";
        m_shader = shader;
        m_gfx = graphics;

        m_num_vertices = vertices.size();

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

        shader->use();

        GLint pos_attrib = glGetAttribLocation(shader->m_shader_program, "position");
        glEnableVertexAttribArray(pos_attrib);
        GLint uv_attrib = glGetAttribLocation(shader->m_shader_program, "uv");
        glEnableVertexAttribArray(uv_attrib);

        glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
        glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)sizeof(vertex::pos));
    }

    mesh::~mesh()
    {
        std::cout << "Mesh destroyed\n";
    }

    void mesh::draw()
    {
        m_shader->use();
        glBindVertexArray(m_vao);
        GLint model_uniform = glGetUniformLocation(m_shader->m_shader_program, "model");
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(m_model_mat));

        GLint view_uniform = glGetUniformLocation(m_shader->m_shader_program, "view");
        glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(m_gfx->m_view_mat));

        GLint proj_uniform = glGetUniformLocation(m_shader->m_shader_program, "project");
        glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(m_gfx->m_projection_mat));

        glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
    }
}