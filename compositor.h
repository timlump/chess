#pragma once
#include "mesh.h"
namespace graphics
{
    class compositor
    {
        public:
            compositor();
            ~compositor();
            void draw();

            std::shared_ptr<shader> m_shader = nullptr;

        private:
            std::shared_ptr<mesh> m_screen_rect = nullptr;
    };
}