#include "console.h"
#include "primitives.h"
#include "utils.h"
#include "binding.h"

namespace console
{
    console* console::s_console = nullptr;

    console* console::get()
    {
        if (s_console == nullptr)
        {
            s_console = new console();
        }
        return s_console;
    }

    void console::release()
    {
        if (s_console != nullptr)
        {
            delete s_console;
            s_console = nullptr;
        }
    }

    console::console()
    {
        m_screen_rect = std::make_shared<graphics::vertex_mesh_data>(primitives::SQUARE3);

        glGenTextures(1, &m_text_texture);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_text_texture);

        auto result = utils::text_to_image(m_current_line, 1024, 256, 32);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 256, 0, GL_RED, GL_UNSIGNED_BYTE, result.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    }

    console::~console()
    {
        glDeleteTextures(1, &m_text_texture);
    }

    void console::draw()
    {
        if (m_shader && m_visible)
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            m_screen_rect->refresh(m_shader);

            GLint colour_location = glGetUniformLocation(m_shader->m_shader_program, "colour_tex");

            if (colour_location >= 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_text_texture);
                std::string console_text = m_current_line;
                for (int idx = 0 ; idx < m_history.size() ; idx++) {
                    console_text += "\n" + m_history[idx];
                }

                auto result = utils::text_to_image(console_text, 1024, 256, 32);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 256, 0, GL_RED, GL_UNSIGNED_BYTE, result.data());
                glUniform1i(colour_location, 0);
            }

            m_screen_rect->draw();
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }
    }

    void console::insert_character(unsigned int codepoint)
    {
        if (not m_visible) {
            return;
        }

        // only support ascii
        if (codepoint > 0xFF) {
            return;
        }

        m_current_line += static_cast<char>(codepoint);
    }

    void console::backspace()
    {
        if (not m_visible) {
            return;
        }

        if (not m_current_line.empty()) {
            m_current_line = m_current_line.substr(0, m_current_line.size() - 1);
        }
    }

    void console::enter()
    {
        if (not m_visible) {
            return;
        }

        int idx = m_history.size() - 2;
        while(idx > 0) {
            m_history[idx] = m_history[idx - 1];
            idx--;
        }

        m_history[0] = m_current_line;
        binding::lua::get()->execute_interactive(m_current_line);
        m_current_line = "";
    }
}