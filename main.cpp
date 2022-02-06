#include "binding.h"

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

struct
{
    lua_State * lua_state = nullptr;
    int width = 800;
    int height = 600;
    GLFWwindow * window = nullptr;
} game_state;

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

void setup_subsystems();
void setup_game();
void game_loop();

int main()
{
    // hook up functions to lua
    {
        graphics::shader::register_lua_functions();
        graphics::mesh_instance::register_lua_functions();
    }
    

    setup_subsystems();
    graphics::scene::get(); // call this to create the singleton

    binding::lua::get()->execute("scripts/init.lua");

    setup_game();
    game_loop();

    binding::lua::release();

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
            graphics::shader::reload_shaders();
        } break;

        case GLFW_KEY_UP:
        {
            graphics::scene::get()->m_light_params.position.z -= 0.01f;
        } break;

        case GLFW_KEY_DOWN:
        {
            graphics::scene::get()->m_light_params.position.z += 0.01f;
        } break;

        case GLFW_KEY_LEFT:
        {
            graphics::scene::get()->m_light_params.position.x -= 0.01f;
        } break;

        case GLFW_KEY_RIGHT:
        {
            graphics::scene::get()->m_light_params.position.x += 0.01f;
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
    if (camera_state.fov < 10.0) {
        camera_state.fov = 10.0;
    }
    
    if (camera_state.fov > 200.0) {
        camera_state.fov = 200.0;
    }
}

void setup_subsystems()
{
    int width = game_state.width;
    int height = game_state.height;
    if (not glfwInit()) {
        throw std::runtime_error("unable to initialize glfw");
    }

    glfwSetErrorCallback(error_callback);

    game_state.window = glfwCreateWindow(game_state.width, game_state.height, "Chess", nullptr, nullptr);
    if (not game_state.window) {
        glfwTerminate();
        throw std::runtime_error("unable to create window");
    }

    glfwMakeContextCurrent(game_state.window);
    glfwSetKeyCallback(game_state.window, key_callback);
    glfwSetCursorPosCallback(game_state.window, mouse_pos_callback);
    glfwSetMouseButtonCallback(game_state.window, mouse_button_callback);
    glfwSetScrollCallback(game_state.window, scroll_callback);
}

void setup_game()
{
    auto scene_ctx = graphics::scene::get();

    // chess::board_state initial_board;
    // chess::piece_colour current_player = chess::white;
    // std::vector<piece> white_pieces;
    // std::vector<piece> black_pieces;
    
    // auto board = game_state.mesh_instances["board"];

    // float board_side_width = board->m_mesh->m_max_dims.x - board->m_mesh->m_min_dims.x;
    // float tile_width = board_side_width / 8;

    // board is already setup, use its state to create the pieces and position them properly
    /*
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
                        mesh_instance->m_mesh = game_state.meshes["pawn"];
                    } break;

                    case chess::bishop:
                    {
                        mesh_instance->m_mesh = game_state.meshes["bishop"];
                    } break;

                    case chess::rook:
                    {
                        mesh_instance->m_mesh = game_state.meshes["rook"];
                    } break;

                    case chess::knight:
                    {
                        mesh_instance->m_mesh = game_state.meshes["knight"];
                        if (colour == chess::black) {
                            mesh_instance->m_y_rotation = glm::radians(180.f);
                        }
                    } break;

                    case chess::queen:
                    {
                        mesh_instance->m_mesh = game_state.meshes["queen"];
                    } break;

                    case chess::king:
                    {
                        mesh_instance->m_mesh = game_state.meshes["king"];
                    } break;

                    default:
                        mesh_instance->m_mesh = game_state.meshes["unknown"];
                };

                mesh_instance->m_colour = (colour == chess::black) ?
                    glm::vec3(0.4, 0.2, 0.0) : glm::vec3(0.67, 0.64, 0.41);

                float piece_width = mesh_instance->m_mesh->m_max_dims.x - mesh_instance->m_mesh->m_min_dims.x;

                mesh_instance->m_shaders_layers[0] = graphics::shader::get_shader("piece");
                mesh_instance->m_shaders_layers[1] = graphics::shader::get_shader("shadow");

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
    */
}

void game_loop()
{
    graphics::compositor compositor;
    compositor.m_shader = graphics::shader::get_shader("passthrough");
    auto scene_ctx = graphics::scene::get();

    int frames = 0;
    double t0 = glfwGetTime();
    while(not glfwWindowShouldClose(game_state.window)) {
        glfwPollEvents();
        if (glfwGetKey(game_state.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(game_state.window, GL_TRUE);
        }

        // game logic here
        scene_ctx->m_camera_params.projection = scene_ctx->m_camera_params.projection = glm::perspective(
            glm::radians(camera_state.fov), game_state.width / (float)game_state.height, 1.f, 10.f
        );

        scene_ctx->draw(graphics::render_type::shadow_map, 1);    
        scene_ctx->draw(graphics::render_type::gbuffer, 0);
        compositor.draw();

        double t = glfwGetTime();
        if (t - t0 > 1.0) {
            int fps = frames / (t - t0);
            std::string fps_title = "FPS: " + std::to_string(fps);
            glfwSetWindowTitle(game_state.window, fps_title.c_str());
            t0 = t;
            frames = 0;
        }
        frames++;
        
        glfwSwapBuffers(game_state.window);
    }

    glfwTerminate();
}