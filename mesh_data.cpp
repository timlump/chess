#include "mesh_data.h"

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

    vertex_mesh_data::vertex_mesh_data(std::vector<vertex> vertices)
    {
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

    vertex_mesh_data::~vertex_mesh_data()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void vertex_mesh_data::draw()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
    }

    void vertex_mesh_data::refresh(std::shared_ptr<shader> shader)
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
}