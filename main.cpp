#include "chess.h"
#include "gfx.h"

const char* vertex_source = 
    R"glsl(
        #version 300 es

        in vec3 position;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 project;

        void main()
        {
            mat4 mvp = project*view*model;
            gl_Position = mvp*vec4(position, 1.0);
        }
    )glsl";

const char* fragment_source =
    R"glsl(
        #version 300 es

        out mediump vec4 outColor;

        void main()
        {
            outColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )glsl";


int main()
{
    std::shared_ptr<graphics::gfx> gfx = std::make_shared<graphics::gfx>();
    
    std::shared_ptr<graphics::shader> tri_shader;
    {
        std::string v_src = vertex_source;
        std::string f_src = fragment_source;
        tri_shader = std::make_shared<graphics::shader>(v_src, f_src);
    }
                
    std::vector<graphics::vertex> vertices = {
        {glm::vec3(0.0f, 0.5f, 0.0f)},
        {glm::vec3(0.5f,-0.5f, 0.0f)},
        {glm::vec3(-0.5f,-0.5f, 0.0f)}
    };
    auto triangle = std::make_shared<graphics::mesh>(tri_shader, gfx.get(), vertices);
    gfx->add_mesh(triangle);

    while(gfx->draw()) {
        triangle->m_model_mat = glm::rotate(triangle->m_model_mat, glm::radians(1.f), glm::vec3(0,0,1));
        // do game logic here
    }

    glfwTerminate();

    /*
    chess::board board;
    while(true) {
        board.print_board();
        char start_x;
        char end_x;
        int start_y;
        int end_y;
        std::cout << "Please enter move: e.g. a 1 a 2 \n";
        std::cin >> start_x >> start_y >> end_x >> end_y;
        start_x -= 97;
        end_x -= 97;
        board.move_piece(start_x, start_y, end_x, end_y);
    }
    */

    return 0;
}