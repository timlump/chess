#include "chess.h"
#include "gfx.h"

int main()
{
    gfx::gfx gfx;
    
    std::shared_ptr<gfx::shader> tri_shader;
    {
        std::string v_src = gfx::vertex_source;
        std::string f_src = gfx::fragment_source;
        tri_shader = std::make_shared<gfx::shader>(v_src, f_src);
    }
                
    std::vector<gfx::vertex> vertices = {
        {0.0f, 0.5f}, {0.5f,-0.5f},{-0.5f,-0.5f}
    };
    auto triangle = std::make_shared<gfx::mesh>(tri_shader,vertices);
    gfx.add_mesh(triangle);

    while(gfx.draw()) {
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