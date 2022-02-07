#include <string>
#include <vector>
namespace utils
{
    std::vector<unsigned char> text_to_image(std::string text,
        int width, int height, int line_height,
        std::string font = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
}