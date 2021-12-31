#include "compositor.h"
#include "primitives.h"
#include "scene.h"

namespace graphics
{
    compositor::compositor()
    {
        m_screen_rect = std::make_shared<mesh>(primitives::SQUARE2);
    }

    compositor::~compositor()
    {

    }

    void compositor::draw()
    {
        if (m_shader) {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            m_screen_rect->refresh(m_shader);

            GLint colour_location = glGetUniformLocation(m_shader->m_shader_program, "colour_tex");

            if (colour_location >= 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, scene::get()->m_colour_tex);
                glUniform1i(colour_location, 0);
            }

            GLint normal_location = glGetUniformLocation(m_shader->m_shader_program, "normal_tex");

            if (normal_location >= 0) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, scene::get()->m_normal_tex);
                glUniform1i(normal_location, 1);
            }

            GLint position_location = glGetUniformLocation(m_shader->m_shader_program, "position_tex");

            if (position_location >= 0) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, scene::get()->m_position_tex);
                glUniform1i(normal_location, 2);
            }

            m_screen_rect->draw();
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }
    }
}