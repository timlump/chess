#include "chess.h"
#include "scene.h"
#include "primitives.h"
#include "compositor.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// callbacks
void error_callback(int error, const char * description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_pos_callback(GLFWwindow* window, double x, double y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// state
struct
{
    float fov = 45.f;
} camera_state;

struct
{
    double x = 0;
    double y = 0;
    
    bool left_button = false;
    bool right_button = false;
    bool middle_button = false;

} mouse_state;

struct piece
{
    int x;
    int y;
    chess::piece_type type;
    chess::piece_colour colour;
    std::shared_ptr<graphics::mesh_instance> instance;
};

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

    g_shaders["ssr"] = std::make_shared<graphics::shader>(
        "shaders/passthrough.vert", "shaders/ssr.frag"  
    );
}
void reload_shaders()
{
    for (auto& shader : g_shaders) {
        shader.second->reload();
    }
}

std::map<std::string, std::shared_ptr<graphics::mesh>> g_meshes;
void load_meshes()
{
    g_meshes["board"] = std::make_shared<graphics::mesh>(
        graphics::load_vertices_obj("meshes/plane.obj", glm::vec3(0.05f))
    );

    g_meshes["pawn"] = std::make_shared<graphics::mesh>(
        graphics::load_vertices_obj("meshes/pawn.obj", glm::vec3(0.05f))
    );

    g_meshes["bishop"] = std::make_shared<graphics::mesh>(
        graphics::load_vertices_obj("meshes/bishop.obj", glm::vec3(0.05f))
    );


    g_meshes["rook"] = std::make_shared<graphics::mesh>(
        graphics::load_vertices_obj("meshes/rook.obj", glm::vec3(0.05f))
    );

    g_meshes["unknown"] = std::make_shared<graphics::mesh>(
        graphics::load_vertices_obj("meshes/unknown.obj", glm::vec3(0.05f))
    );

    g_meshes["cube"] = std::make_shared<graphics::mesh>(
        graphics::load_vertices_obj("meshes/cube.obj", glm::vec3(0.05f))
    );
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

    graphics::scene::create(width,height);
    auto scene_ctx = graphics::scene::get();

    load_shaders();
    load_meshes();

    chess::board_state initial_board;
    chess::piece_colour current_player = chess::white;
    std::vector<piece> white_pieces;
    std::vector<piece> black_pieces;

    auto cube = std::make_shared<graphics::mesh_instance>();
    {
        cube->m_mesh = g_meshes["cube"];
        cube->add_shader(g_shaders["piece"]);
        //cube->m_position = glm::vec3(0,5,0);
    }
    scene_ctx->add_mesh(cube);
    
    auto board = std::make_shared<graphics::mesh_instance>();
    {
        board->m_mesh = g_meshes["board"];
        board->add_shader(g_shaders["board"]);
    }

    float board_side_width = board->m_mesh->m_max_dims.x - board->m_mesh->m_min_dims.x;
    float tile_width = board_side_width / 8;

    scene_ctx->add_mesh(board);

    auto board2 = std::make_shared<graphics::mesh_instance>();
    {
        board2->m_mesh = g_meshes["board"];
        board2->add_shader(g_shaders["board"]);
        board2->m_x_rotation = glm::radians(90.f);
    }
    scene_ctx->add_mesh(board2);

    auto board3 = std::make_shared<graphics::mesh_instance>();
    {
        board3->m_mesh = g_meshes["board"];
        board3->add_shader(g_shaders["board"]);
        board3->m_x_rotation = glm::radians(90.f);
        board3->m_y_rotation = glm::radians(90.f);
    }
    scene_ctx->add_mesh(board3);

    // board is already setup, use its state to create the pieces and position them properly
    
    for (int y = 0 ; y < 8 ; y++)
    {
        for (int x = 0 ; x < 8 ; x++)
        {
            uint8_t tile = initial_board.tiles[y][x];
            if (tile) {
                std::vector<piece>& player_pieces = (tile & chess::black) ? black_pieces : white_pieces;
                uint8_t colour = tile & 0b1000;
                uint8_t type = tile & 0b111;

                piece current_piece;
                current_piece.x = x;
                current_piece.y = y;

                auto mesh_instance = std::make_shared<graphics::mesh_instance>();
                switch (type) {
                    case chess::pawn:
                    {
                        mesh_instance->m_mesh = g_meshes["pawn"];
                    } break;

                    case chess::bishop:
                    {
                        mesh_instance->m_mesh = g_meshes["bishop"];
                    } break;

                    case chess::rook:
                    {
                        mesh_instance->m_mesh = g_meshes["rook"];
                    } break;

                    default:
                        mesh_instance->m_mesh = g_meshes["unknown"];
                };
                float piece_width = mesh_instance->m_mesh->m_max_dims.x - mesh_instance->m_mesh->m_min_dims.x;

                mesh_instance->add_shader(g_shaders["piece"]);
                mesh_instance->add_shader(g_shaders["id"], 1);
                mesh_instance->m_position.x = 
                    (tile_width*x) + (piece_width/2) - (board_side_width/2);
                mesh_instance->m_position.z =
                    (tile_width*y) + (piece_width/2) - (board_side_width/2);

                current_piece.instance = mesh_instance;
                scene_ctx->add_mesh(current_piece.instance);

                player_pieces.push_back(current_piece);
            }
        }
    }
    

    scene_ctx->m_view_mat = glm::lookAt(glm::vec3(1.2f, 1.2f, 1.2f), glm::vec3(0.f), glm::vec3(0.f,1.f,0.f));

    graphics::compositor compositor;
    compositor.m_shader = g_shaders["ssr"];

    while(not glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // game logic here
        scene_ctx->m_projection_mat = glm::perspective(
            glm::radians(camera_state.fov), width / (float)height, 1.f, 10.f
        );

        //board->rotate(1.f, glm::vec3(0,1,0));
        //piece->translate(glm::vec3(0,-0.004f, 0));
        //reflected_piece->translate(glm::vec3(0,-0.004f, 0));

        // mouse picking
        // if (mouse_state.left_button) {
        //     glm::vec3 old_clear_colour = scene_ctx->m_clear_colour;
        //     scene_ctx->m_clear_colour = glm::vec3(0);
        //     scene_ctx->draw(1, true);
        //     scene_ctx->m_clear_colour = old_clear_colour;
        //     uint8_t colour[3] = {};
        //     glReadPixels(
        //         static_cast<GLint>(mouse_state.x),
        //         height - static_cast<GLint>(mouse_state.y),
        //         1, 1, GL_RGB, GL_UNSIGNED_BYTE, colour
        //     );
        //     // convert back to id
        //     uint32_t id = 0;
        //     id = (colour[2] << 16) | (colour[1] << 8) | (colour[0]);
        //     std::cout << "id: " << id << std::endl;
        // }

        scene_ctx->draw(0, true);
        compositor.draw();
        
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

void error_callback(int error, const char * description)
{
    std::cerr << "Error: " << error << std::endl;
    std::cerr << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key) {
        case GLFW_KEY_R:
        {
            reload_shaders();
        } break;
    }
}

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