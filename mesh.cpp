#include "mesh.h"
#include "gfx.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace graphics
{
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
        glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
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