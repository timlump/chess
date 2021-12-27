#include "gfx.h"
#include "primitives.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void error_callback(int error, const char * description)
{
    std::cerr << "Error: " << error << std::endl;
    std::cerr << description << std::endl;
}

struct
{
    float fov = 45.f;
} camera_state;

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera_state.fov -= yoffset;
}

std::map<std::string, std::shared_ptr<graphics::shader>> g_shaders;
void load_shaders()
{
    g_shaders["id"] = std::make_shared<graphics::shader>(
        "shaders/id.vert", "shaders/id.frag"
    );

    g_shaders["board"] = std::make_shared<graphics::shader>(
        "shaders/checker.vert", "shaders/checker.frag"
    );

    g_shaders["piece"] = std::make_shared<graphics::shader>(
        "shaders/piece.vert", "shaders/piece.frag"
    );

    g_shaders["reflected_piece"] = std::make_shared<graphics::shader>(
        "shaders/piece_reflected.vert", "shaders/piece_reflected.frag"
    );
}

std::map<std::string, std::shared_ptr<graphics::mesh>> g_meshes;
void load_meshes()
{
    g_meshes["board"] = std::make_shared<graphics::mesh>(
        primitives::SQUARE
    );

    g_meshes["pawn"] = std::make_shared<graphics::mesh>(
        graphics::load_vertices_obj("meshes/pawn.obj")
    );
}

void setup_reflector()
{
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);
    glDepthMask(GL_FALSE);
    glClear(GL_STENCIL_BUFFER_BIT);
}

void cleanup_reflector()
{
    glDisable(GL_STENCIL_TEST);
}

void setup_reflected()
{
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDepthMask(GL_TRUE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glFrontFace(GL_CW);
}

void cleanup_reflected()
{
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glFrontFace(GL_CCW);
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
    glfwSetScrollCallback(window, scroll_callback);

    graphics::gfx::create(width,height);
    auto gfx = graphics::gfx::get();

    load_shaders();
    load_meshes();

    gfx->m_view_mat = glm::lookAt(glm::vec3(1.2f, 1.2f, 1.2f), glm::vec3(0.f), glm::vec3(0.f,1.f,0.f));
    
    auto board = std::make_shared<graphics::mesh_instance>();
    {
        board->m_mesh = g_meshes["board"];
        board->add_shader(g_shaders["board"]);
        board->on_begin_draw = setup_reflector;
        board->on_finish_draw = cleanup_reflector;
    }

    gfx->add_mesh(board,graphics::render_order::reflector);

    auto piece = std::make_shared<graphics::mesh_instance>();
    {
        piece->m_mesh = g_meshes["pawn"];
        piece->add_shader(g_shaders["piece"], 0);
        piece->add_shader(g_shaders["id"], 1);
        piece->scale(glm::vec3(0.1, 0.1, 0.1));
        piece->translate(glm::vec3(0,5,0));
    }
    
    gfx->add_mesh(piece);

    auto reflected_piece = std::make_shared<graphics::mesh_instance>();
    {
        reflected_piece->m_mesh = g_meshes["pawn"];
        reflected_piece->add_shader(g_shaders["reflected_piece"]);
        reflected_piece->scale(glm::vec3(0.1, -0.1, 0.1));
        reflected_piece->translate(glm::vec3(0,5,0));
        reflected_piece->on_begin_draw = setup_reflected;
        reflected_piece->on_finish_draw = cleanup_reflected;
    }

    gfx->add_mesh(reflected_piece, graphics::render_order::reflected);

    while(not glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // game logic here
        gfx->m_projection_mat = glm::perspective(
            glm::radians(camera_state.fov), width / (float)height, 1.f, 10.f
        );

        board->rotate(1.f, glm::vec3(0,1,0));
        piece->translate(glm::vec3(0,-0.004f, 0));
        reflected_piece->translate(glm::vec3(0,-0.004f, 0));

        // mouse picking
        if (mouse_state.left_button) {
            glm::vec3 old_clear_colour = gfx->m_clear_colour;
            gfx->m_clear_colour = glm::vec3(0);
            gfx->draw(1, true);
            gfx->m_clear_colour = old_clear_colour;
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

    return 0;
}