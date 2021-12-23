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
        glEnable(GL_CULL_FACE);  
    }

    gfx::~gfx() {
        m_meshes.clear();
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
            mesh.second->draw();
        }
    }

    void gfx::add_mesh(std::shared_ptr<mesh> mesh, int order) {
        m_meshes.insert(std::make_pair(order, mesh));
    }
}