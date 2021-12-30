#include "compositor.h"
#include "primitives.h"

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
            m_screen_rect->draw();
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }
    }
}