#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>

namespace gfx
{
    void error_callback(int error, const char * description)
    {
        std::cerr << "Error: " << error << std::endl;
        std::cerr << description << std::endl;
    }

    const char* vertex_source = 
    R"glsl(
        #version 300 es

        in vec2 position;

        void main()
        {
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )glsl";

    const char* fragment_source =
    R"glsl(
        #version 300 es

        out mediump vec4 outColor;

        void main()
        {
            outColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )glsl";

    struct vertex
    {
        float x;
        float y;
    };

    class shader {
        public:
            shader(std::string vert_source, std::string frag_source) {
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

            ~shader() {
                std::cout << "Shader destroyed\n";
            }

            void use()
            {
                glUseProgram(m_shader_program);
            }


            GLuint m_shader_program;

        private:
            GLuint m_vertex_shader;
            GLuint m_fragment_shader;
    };

    class mesh {
        public:
            mesh(std::shared_ptr<shader> shader,std::vector<vertex> vertices) {
                std::cout << "Mesh created\n";
                m_shader = shader;

                m_num_vertices = vertices.size();

                glGenVertexArrays(1, &m_vao);
                glBindVertexArray(m_vao);

                glGenBuffers(1, &m_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

                shader->use();

                GLint pos_attrib = glGetAttribLocation(shader->m_shader_program, "position");
                glEnableVertexAttribArray(pos_attrib);
                glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
            }

            ~mesh()
            {
                std::cout << "Mesh destroyed\n";
            }

            void draw()
            {
                m_shader->use();
                glBindVertexArray(m_vao);
                glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
            }

        private:
            std::shared_ptr<shader> m_shader;
            int m_num_vertices;
            GLuint m_vbo;
            GLuint m_vao;
    };

    class gfx {
        public:
            gfx() {
                if (not glfwInit()) {
                    throw std::runtime_error("unable to initialize glfw");
                }

                glfwSetErrorCallback(error_callback);

                m_window = glfwCreateWindow(640, 480, "Chess", nullptr, nullptr);
                if (not m_window) {
                    glfwTerminate();
                    throw std::runtime_error("unable to create window");
                }

                glfwMakeContextCurrent(m_window);

                // this must be called after getting the opengl context
                glewExperimental = GL_TRUE;
                if (glewInit() != GLEW_OK) {
                    throw std::runtime_error("unable to initialize glew");
                }
            }

            ~gfx() {

            }

            bool draw() {
                GLenum error = glGetError();
                if (error != GL_NO_ERROR)
                {
                    const char* error_string = reinterpret_cast<const char*>(glewGetErrorString(error));
                    std::cerr << "OpenGL error: " << error_string << std::endl;
                }

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                
                for (auto& mesh : m_meshes) {
                    mesh->draw();
                }

                glfwSwapBuffers(m_window);
                glfwPollEvents();

                if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    glfwSetWindowShouldClose(m_window, GL_TRUE);
                }

                return not glfwWindowShouldClose(m_window);
            }

            void add_mesh(std::shared_ptr<mesh> mesh) {
                m_meshes.push_back(mesh);
            }

            void remove_mesh(std::shared_ptr<mesh> mesh) {
                auto find_iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
                if (find_iter != m_meshes.end()) {
                    m_meshes.erase(find_iter);
                }
            }

        private:
            GLFWwindow *m_window = nullptr;
            std::vector<std::shared_ptr<mesh>> m_meshes;
    };
}