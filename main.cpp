#include "chess.h"
#include "gfx.h"

int main()
{
    int width = 640;
    int height = 480;
    std::shared_ptr<graphics::gfx> gfx = std::make_shared<graphics::gfx>(width, height);
    gfx->m_view_mat = glm::lookAt(glm::vec3(1.2f, 1.2f, 1.2f), glm::vec3(0.f), glm::vec3(0.f,0.f,1.f));
    gfx->m_projection_mat = glm::perspective(glm::radians(45.f), width / (float)height, 1.f, 10.f);
    
    std::shared_ptr<graphics::shader> tri_shader;
    {
        tri_shader = std::make_shared<graphics::shader>("shaders/basic.vert", "shaders/basic.frag");
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