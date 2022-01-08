#include "scene.h"

#include <iostream>
#include <algorithm>

namespace graphics
{
    scene* scene::s_gfx = nullptr;

    void scene::destroy()
    {
        if (s_gfx) {
            delete s_gfx;
            s_gfx = nullptr;
        }
    }

    scene* scene::get()
    {
        if (s_gfx == nullptr) {
            s_gfx = new scene();  
        }
        return s_gfx;
    }
    
    scene::scene() {
        // this must be called after getting the opengl context
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            throw std::runtime_error("unable to initialize glew");
        }

        {
            GLint dimensions[4] = {};
            glGetIntegerv(GL_VIEWPORT, dimensions);
            m_width = dimensions[2];
            m_height = dimensions[3];
            std::cout << "Screen: " << dimensions[2] << " " << dimensions[3] << std::endl;
        }
        
        {
            GLint max_attachments = 0;
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_attachments);

            GLint max_draw_buffers = 0;
            glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);

            std::cout << "Max colour attachments: " << max_attachments << std::endl;
            std::cout << "Max draw buffers: " << max_draw_buffers << std::endl;
        }

        // gbuffer
        {
            glGenFramebuffers(1, &m_gbuffer_framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_framebuffer);

            // colour buffer
            {
                glGenTextures(1, &m_colour_tex);
                glBindTexture(GL_TEXTURE_2D, m_colour_tex);

                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colour_tex, 0
                );
            }

            // normal buffer
            {
                glGenTextures(1, &m_normal_tex);
                glBindTexture(GL_TEXTURE_2D, m_normal_tex);

                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal_tex, 0
                );
            }

            // position buffer
            {
                glGenTextures(1, &m_position_tex);
                glBindTexture(GL_TEXTURE_2D, m_position_tex);

                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_position_tex, 0
                );
            }

            glGenRenderbuffers(1, &m_depth_stencil_rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);

            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_rbo
            );

            const GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
            glDrawBuffers(3, buffers);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("framebuffer incomplete");
            }
        }

        // shadow map
        {
            glGenFramebuffers(1, &m_shadowmap_framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmap_framebuffer);

            glGenTextures(1, &m_shadowmap_tex);
            glBindTexture(GL_TEXTURE_2D, m_shadowmap_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowmap_tex, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("framebuffer incomplete");
            }
        }

        // restore normal framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    scene::~scene() {
        m_meshes.clear();

        glDeleteTextures(1, &m_colour_tex);
        glDeleteTextures(1, &m_normal_tex);
        glDeleteTextures(1, &m_position_tex);
        glDeleteTextures(1, &m_shadowmap_tex);
        glDeleteRenderbuffers(1, &m_depth_stencil_rbo);
        glDeleteFramebuffers(1, &m_gbuffer_framebuffer);
        glDeleteFramebuffers(1, &m_shadowmap_framebuffer);
    }

    void scene::draw(render_type type, int layer) {
        glViewport(0,0,m_width,m_height);
        switch (type)
        {
            case render_type::standard:
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            } break;

            case render_type::gbuffer:
            {
                glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_framebuffer);
            } break;
            
            case render_type::shadow_map:
            {
                glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
                glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmap_framebuffer);
            } break;
        }

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            const char* error_string = reinterpret_cast<const char*>(glewGetErrorString(error));
            std::cerr << "OpenGL error: (" << error << ") " << error_string << std::endl;
        }

        glClearColor(m_clear_colour.r, m_clear_colour.g, m_clear_colour.b, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_shadowmap_tex);
        
        for (auto& mesh : m_meshes) {
            mesh.second->draw(layer);
        }

        // restore framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,m_width,m_height);
    }

    void scene::add_mesh(std::shared_ptr<mesh_instance> instance, int order) {
        m_meshes.insert(std::make_pair(order, instance));
    }
}