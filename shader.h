#pragma once
#include <map>
#include <memory>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace graphics
{
    class shader {
    public:
        static void register_lua_functions();
        inline static std::map<std::string, std::shared_ptr<graphics::shader>> m_shaders;

        shader(std::string vert_path, std::string frag_path);

        ~shader();

        void reload();

        void use();

        GLuint m_shader_program;
        std::string m_vert_path;
        std::string m_frag_path;

        int m_id;

    private:
        void load();
        void release();
        GLuint m_vertex_shader;
        GLuint m_fragment_shader;
    };
}