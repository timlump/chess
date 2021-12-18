#include "gfx.h"

#include <iostream>
#include <algorithm>

namespace graphics
{
    void error_callback(int error, const char * description)
    {
        std::cerr << "Error: " << error << std::endl;
        std::cerr << description << std::endl;
    }

    gfx::gfx(int width, int height) {
        if (not glfwInit()) {
            throw std::runtime_error("unable to initialize glfw");
        }

        glfwSetErrorCallback(error_callback);

        m_window = glfwCreateWindow(width, height, "Chess", nullptr, nullptr);
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

    gfx::~gfx() {

    }

    bool gfx::draw() {
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

        return !glfwWindowShouldClose(m_window);
    }

    void gfx::add_mesh(std::shared_ptr<mesh> mesh) {
        m_meshes.push_back(mesh);
    }

    void gfx::remove_mesh(std::shared_ptr<mesh> mesh) {
        auto find_iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
        if (find_iter != m_meshes.end()) {
            m_meshes.erase(find_iter);
        }
    }
}