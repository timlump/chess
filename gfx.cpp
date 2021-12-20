#include "gfx.h"

#include <iostream>
#include <algorithm>

namespace graphics
{
    
    gfx::gfx() {
        // this must be called after getting the opengl context
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            throw std::runtime_error("unable to initialize glew");
        }

        glEnable(GL_DEPTH_TEST);
    }

    gfx::~gfx() {

    }

    void gfx::draw() {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            const char* error_string = reinterpret_cast<const char*>(glewGetErrorString(error));
            std::cerr << "OpenGL error: " << error_string << std::endl;
        }

        glClearColor(m_clear_colour.r, m_clear_colour.g, m_clear_colour.b, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        for (auto& mesh : m_meshes) {
            mesh->draw();
        }
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