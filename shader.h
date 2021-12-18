#pragma once
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace graphics
{
    class shader {
    public:
        shader(std::string vert_source, std::string frag_source);

        ~shader();

        void use();

        GLuint m_shader_program;

    private:
        GLuint m_vertex_shader;
        GLuint m_fragment_shader;
    };
}