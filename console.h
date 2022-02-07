#include "mesh.h"

namespace console
{
    class console
    {
        public:
            static console* get();
            static void release();

            void draw();

            bool m_visible = false;
            std::shared_ptr<graphics::shader> m_shader = nullptr;

            void insert_character(unsigned int codepoint);
            void backspace();
            void enter();

        private:
            GLuint m_text_texture;

            static console *s_console;
            console();
            ~console();
            std::shared_ptr<graphics::mesh_data> m_screen_rect = nullptr;

            std::string m_current_line = "";
            std::vector<std::string> m_history = {"", "", "", "", "", ""};
    };
}