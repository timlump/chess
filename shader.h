#pragma once
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace graphics
{
    class shader {
    public:
        shader(std::string vert_path, std::string frag_path);

        ~shader();

        void reload();

        void use();

        GLuint m_shader_program;
        std::string m_vert_path;
        std::string m_frag_path;

    private:
        void load();
        void release();
        GLuint m_vertex_shader;
        GLuint m_fragment_shader;
    };
}