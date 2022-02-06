#pragma once
#include <string>
#include <memory>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// forward declare
struct lua_State;

namespace graphics
{
    class shader {
    public:
        static void register_lua_functions();

        shader(std::string vert_path, std::string frag_path);

        ~shader();

        void reload();

        void use();

        GLuint m_shader_program;
        std::string m_vert_path;
        std::string m_frag_path;

        int m_id;

        static void reload_shaders();
        static std::shared_ptr<shader> get_shader(std::string name);

    private:
        void load();
        void release();
        GLuint m_vertex_shader;
        GLuint m_fragment_shader;

        inline static std::map<std::string, std::shared_ptr<shader>> s_shaders;

        // lua functions
        static int load_shader(lua_State* state);
    };
}