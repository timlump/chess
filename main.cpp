#include "chess.h"
#include "gfx.h"
#include "primitives.h"

void error_callback(int error, const char * description)
{
    std::cerr << "Error: " << error << std::endl;
    std::cerr << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modes)
{
}

int main()
{
    int width = 640;
    int height = 480;
    if (not glfwInit()) {
        throw std::runtime_error("unable to initialize glfw");
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow *window = glfwCreateWindow(width, height, "Chess", nullptr, nullptr);
    if (not window) {
        glfwTerminate();
        throw std::runtime_error("unable to create window");
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    std::shared_ptr<graphics::gfx> gfx = std::make_shared<graphics::gfx>();
    gfx->m_view_mat = glm::lookAt(glm::vec3(1.2f, 1.2f, 1.2f), glm::vec3(0.f), glm::vec3(0.f,1.f,0.f));
    gfx->m_projection_mat = glm::perspective(glm::radians(45.f), width / (float)height, 1.f, 10.f);
    
    auto board_shader = std::make_shared<graphics::shader>(
        "shaders/checker.vert", "shaders/checker.frag"
    );
    
    auto board = std::make_shared<graphics::mesh>(board_shader, gfx.get(), primitives::SQUARE);
    board->on_stencil = [](){
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
        glDepthMask(GL_FALSE);
        glClear(GL_STENCIL_BUFFER_BIT);
    };
    gfx->add_mesh(board,graphics::render_order::reflector);

    auto piece_shader = std::make_shared<graphics::shader>(
        "shaders/piece.vert", "shaders/piece.frag"
    );

    auto cube_vertices = graphics::load_vertices_obj("meshes/cube.obj");
    auto piece = std::make_shared<graphics::mesh>(piece_shader, gfx.get(), cube_vertices);
    piece->scale(glm::vec3(0.1, 0.1, 0.1));
    piece->translate(glm::vec3(0,5,0));
    gfx->add_mesh(piece);

    auto reflected_piece = std::make_shared<graphics::mesh>(piece_shader, gfx.get(), cube_vertices);
    reflected_piece->scale(glm::vec3(0.1, -0.1, 0.1));
    reflected_piece->translate(glm::vec3(0,5,0));
    reflected_piece->on_stencil = [](){
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDepthMask(GL_TRUE);
    };
    gfx->add_mesh(reflected_piece, graphics::render_order::reflected);

    while(not glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // game logic here
        board->rotate(1.f, glm::vec3(0,1,0));

        gfx->draw();
        glfwSwapBuffers(window);
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