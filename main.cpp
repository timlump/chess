#include "chess.h"
#include "gfx.h"
#include "primitives.h"
#include <glm/gtc/type_ptr.hpp>

void error_callback(int error, const char * description)
{
    std::cerr << "Error: " << error << std::endl;
    std::cerr << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

struct
{
    double x = 0;
    double y = 0;
    
    bool left_button = false;
    bool right_button = false;
    bool middle_button = false;

} mouse_state;

void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    mouse_state.x = x;
    mouse_state.y = y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    switch(button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            mouse_state.left_button = (action == GLFW_PRESS);
        } break;

        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            mouse_state.right_button = (action == GLFW_PRESS);
        } break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
        {
            mouse_state.middle_button = (action == GLFW_PRESS);
        } break;
    }
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
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    std::shared_ptr<graphics::gfx> gfx = std::make_shared<graphics::gfx>(width,height);
    gfx->m_view_mat = glm::lookAt(glm::vec3(1.2f, 1.2f, 1.2f), glm::vec3(0.f), glm::vec3(0.f,1.f,0.f));
    gfx->m_projection_mat = glm::perspective(glm::radians(45.f), width / (float)height, 1.f, 10.f);
    
    auto board_shader = std::make_shared<graphics::shader>(
        "shaders/checker.vert", "shaders/checker.frag"
    );
    
    auto board = std::make_shared<graphics::mesh>(gfx.get(), primitives::SQUARE);
    board->add_shader(board_shader);
    board->on_begin_draw = [](){
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
        glDepthMask(GL_FALSE);
        glClear(GL_STENCIL_BUFFER_BIT);
    };
    board->on_finish_draw = [](){
        glDisable(GL_STENCIL_TEST);
    };
    gfx->add_mesh(board,graphics::render_order::reflector);

    auto id_shader = std::make_shared<graphics::shader>(
        "shaders/id.vert", "shaders/id.frag"
    );

    auto piece_shader = std::make_shared<graphics::shader>(
        "shaders/piece.vert", "shaders/piece.frag"
    );

    auto reflected_piece_shader = std::make_shared<graphics::shader>(
        "shaders/piece_reflected.vert", "shaders/piece_reflected.frag"
    );

    auto cube_vertices = graphics::load_vertices_obj("meshes/pawn.obj");
    auto piece = std::make_shared<graphics::mesh>(gfx.get(), cube_vertices);
    piece->add_shader(piece_shader, 0);
    piece->add_shader(id_shader, 1);
    piece->scale(glm::vec3(0.1, 0.1, 0.1));
    piece->translate(glm::vec3(0,5,0));
    gfx->add_mesh(piece);

    auto reflected_piece = std::make_shared<graphics::mesh>(gfx.get(), cube_vertices);
    reflected_piece->add_shader(reflected_piece_shader);
    reflected_piece->scale(glm::vec3(0.1, -0.1, 0.1));
    reflected_piece->translate(glm::vec3(0,5,0));
    reflected_piece->on_begin_draw = [](){
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);

        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDepthMask(GL_TRUE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glFrontFace(GL_CW);
    };
    reflected_piece->on_finish_draw = [](){
        glDisable(GL_BLEND);
        glDisable(GL_STENCIL_TEST);
        glFrontFace(GL_CCW);
    };

    gfx->add_mesh(reflected_piece, graphics::render_order::reflected);

    while(not glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // game logic here
        board->rotate(1.f, glm::vec3(0,1,0));
        piece->translate(glm::vec3(0,-0.004f, 0));
        reflected_piece->translate(glm::vec3(0,-0.004f, 0));

        // mouse picking
        if (mouse_state.left_button) {
            gfx->draw(1, true);
            uint8_t colour[3] = {};
            glReadPixels(
                static_cast<GLint>(mouse_state.x),
                height - static_cast<GLint>(mouse_state.y),
                1, 1, GL_RGB, GL_UNSIGNED_BYTE, colour
            );
        }

        // regular drawing
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