#include "chess.h"
#include <GLFW/glfw3.h>

int main()
{
    GLFWwindow * window = nullptr;
    if (not glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (not window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (not glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
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