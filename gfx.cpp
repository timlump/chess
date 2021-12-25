#include "gfx.h"

#include <iostream>
#include <algorithm>

namespace graphics
{
    
    gfx::gfx(int width, int height) {
        // this must be called after getting the opengl context
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            throw std::runtime_error("unable to initialize glew");
        }

        // setup frame buffer for id buffer (and other effects)
        glGenFramebuffers(1, &m_render_tex_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_render_tex_framebuffer);

        glGenTextures(1, &m_render_tex);
        glBindTexture(GL_TEXTURE_2D, m_render_tex);

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_render_tex, 0
        );

        glGenRenderbuffers(1, &m_depth_stencil_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_rbo
        );

        // restore normal framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);  
    }

    gfx::~gfx() {
        m_meshes.clear();

        glDeleteTextures(1, &m_render_tex);
        glDeleteRenderbuffers(1, &m_depth_stencil_rbo);
        glDeleteFramebuffers(1, &m_render_tex_framebuffer);
    }

    void gfx::draw(int layer, bool render_to_texture) {
        if (render_to_texture) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_render_tex_framebuffer);
        }
        else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            const char* error_string = reinterpret_cast<const char*>(glewGetErrorString(error));
            std::cerr << "OpenGL error: " << error_string << std::endl;
        }

        glClearColor(m_clear_colour.r, m_clear_colour.g, m_clear_colour.b, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        for (auto& mesh : m_meshes) {
            mesh.second->draw(layer);
        }
    }

    void gfx::add_mesh(std::shared_ptr<mesh> mesh, int order) {
        m_meshes.insert(std::make_pair(order, mesh));
    }
}