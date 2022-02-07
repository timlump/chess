#include "utils.h"

extern "C" {
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"

    #define STB_TRUETYPE_IMPLEMENTATION
    #include "stb_truetype.h"
}

#include <fstream>

namespace utils
{
    std::vector<unsigned char> text_to_image(std::string text, 
        int width, int height, int line_height,
        std::string font)
    {
        std::vector<unsigned char> result(width*height,0x0);

        std::ifstream font_file(font, std::ifstream::binary | std::ifstream::in);
        if (font_file.is_open()) {

            unsigned char * buffer = nullptr;
            // get size of file
            font_file.seekg(0, font_file.end);
            int length = font_file.tellg();
            font_file.seekg(0,font_file.beg);

            buffer = new unsigned char[length];
            font_file.read(reinterpret_cast<char*>(buffer), length);
            font_file.close();

            // setup font
            stbtt_fontinfo font_info;
            stbtt_InitFont(&font_info, buffer, 0);

            // font metrics
            float scale = stbtt_ScaleForPixelHeight(&font_info, line_height);
            int ascent;
            int descent;
            int line_gap;
            stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);
            
            ascent = std::round(ascent*scale);
            descent = std::round(descent*scale);

            int x = 0;
            int line_offset = 0;
            for (int c_idx = 0 ; c_idx < text.size() ; c_idx++)
            {
                int codepoint = text[c_idx];
                if (codepoint == '\n') {
                    line_offset += line_height;
                    x = 0;
                }
                else {
                    int advance;
                    int ls_bearing;
                    stbtt_GetCodepointHMetrics(&font_info, codepoint, &advance, &ls_bearing);

                    int bb_x0;
                    int bb_y0;
                    int bb_x1;
                    int bb_y1;
                    stbtt_GetCodepointBitmapBox(&font_info, codepoint, scale, scale,
                        &bb_x0, &bb_y0, &bb_x1, &bb_y1);

                    int y = ascent + bb_y0 + line_offset;
                    int byte_offset = x + std::round(ls_bearing*scale) + (y*width);
                    stbtt_MakeCodepointBitmap(&font_info, &result[byte_offset],
                        bb_x1 - bb_x0, bb_y1 - bb_y0, width, scale, scale, codepoint);

                    x += std::round(advance*scale);

                    // only do kerning if not last character
                    if (c_idx < text.size() - 1) {
                        int kern = stbtt_GetCodepointKernAdvance(&font_info, codepoint, text[c_idx+1]);
                        x += std::round(kern*scale);
                    }
                }
            }

            delete[] buffer;
        }

        return result;
    }
}