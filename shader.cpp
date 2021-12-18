#include "shader.h"
#include <iostream>
#include <stdexcept>

namespace graphics
{
    shader::shader(std::string vert_source, std::string frag_source) {
        std::cout << "Shader created\n";

        m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        auto vert_source_c = vert_source.c_str();
        auto frag_source_c = frag_source.c_str();
        glShaderSource(m_vertex_shader, 1, &vert_source_c, nullptr);
        glShaderSource(m_fragment_shader, 1, &frag_source_c, nullptr);

        bool successful_compile = true;
        glCompileShader(m_vertex_shader);
        {
            GLint status;
            glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
                char buffer[512];
                glGetShaderInfoLog(m_vertex_shader, 512, nullptr, buffer);
                std::cerr << "Vertex shader error: " << buffer << std::endl;
                successful_compile = false;
            }
        }
        
        glCompileShader(m_fragment_shader);
        {
            GLint status;
            glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
                char buffer[512];
                glGetShaderInfoLog(m_fragment_shader, 512, nullptr, buffer);
                std::cerr << "Fragment shader error: " << buffer << std::endl;
                successful_compile = false;
            }
        }

        if (not successful_compile) {
            throw std::runtime_error("Unable to compile shader");
        }

        m_shader_program = glCreateProgram();
        glAttachShader(m_shader_program, m_vertex_shader);
        glAttachShader(m_shader_program, m_fragment_shader);
        glLinkProgram(m_shader_program);
        glUseProgram(m_shader_program);
    }

    shader::~shader() {
        std::cout << "Shader destroyed\n";
    }

    void shader::use()
    {
        glUseProgram(m_shader_program);
    }
}