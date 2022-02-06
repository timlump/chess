#include "shader.h"
#include "mesh.h"
#include "binding.h"

#include <map>
#include <memory>

#include <iostream>
#include <stdexcept>
#include <fstream>

namespace graphics
{
    // load_shader(name, vert_shader_path, frag_shader_path)
    int shader::load_shader(lua_State* state)
    {
        std::string name = luaL_checkstring(state, 1);
        std::string vert = luaL_checkstring(state, 2);
        std::string frag = luaL_checkstring(state, 3);

        if (s_shaders.find(name) != s_shaders.end()) 
        {
            std::cerr << "Shader: " << name << " already exists\n";
        }
        else {
            auto shader = std::make_shared<graphics::shader>(
                vert, frag  
            );

            s_shaders[name] = shader;
        }
        return 0;
    }
    
    void shader::register_lua_functions()
    {
        binding::lua::get()->bind("load_shader", shader::load_shader);
    }

    std::shared_ptr<shader> shader::get_shader(std::string name)
    {
        auto iter = s_shaders.find(name);
        if (iter != s_shaders.end()) {
            return iter->second;
        }
        return nullptr;
    }

    void shader::reload_shaders()
    {
        for (auto iter : s_shaders)
        {
            iter.second->reload();
        }
    }

    std::string readfile(std::string path)
    {
        std::string result = "";
        std::ifstream file(path);
        if (file.is_open()) {
            while(not file.eof()) {
                std::string line;
                std::getline(file, line);
                result += line + "\n";
            }
            file.close();
        }
        else {
            throw std::runtime_error("unable to load shader");
        }
        return result;
    }

    shader::shader(std::string vert_path, std::string frag_path) {
        // needed for reloading
        m_vert_path = vert_path;
        m_frag_path = frag_path;

        static int id = 0;
        m_id = id;
        id++;

        load();
    }

    shader::~shader() {
        release();
    }

    void shader::reload()
    {
        release();
        load();
    }

    void shader::use()
    {
        glUseProgram(m_shader_program);
    }

    void shader::load()
    {
        m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        {
            std::string vert_source = readfile(m_vert_path);
            std::string frag_source = readfile(m_frag_path);
            
            auto vert_source_c = vert_source.c_str();
            auto frag_source_c = frag_source.c_str();
            glShaderSource(m_vertex_shader, 1, &vert_source_c, nullptr);
            glShaderSource(m_fragment_shader, 1, &frag_source_c, nullptr);
        }

        bool successful_compile = true;
        glCompileShader(m_vertex_shader);
        {
            GLint status;
            glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
                char buffer[512];
                glGetShaderInfoLog(m_vertex_shader, 512, nullptr, buffer);
                std::cerr << m_vert_path << std::endl;
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
                std::cerr << m_frag_path << std::endl;
                std::cerr << "Fragment shader error: " << buffer << std::endl;
                successful_compile = false;
            }
        }

        if (not successful_compile) {
            std::cerr << "Unable to compile shader\n";
            throw std::runtime_error("unable to compile shader\n");
        }

        m_shader_program = glCreateProgram();
        glAttachShader(m_shader_program, m_vertex_shader);
        glAttachShader(m_shader_program, m_fragment_shader);
        glLinkProgram(m_shader_program);
        glUseProgram(m_shader_program);
    }

    void shader::release()
    {
        glDeleteProgram(m_shader_program);
        glDeleteShader(m_fragment_shader);
        glDeleteShader(m_vertex_shader);
    }
}