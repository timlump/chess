#pragma once
#include <stdint.h>
#include <algorithm>

namespace chess
{
    enum piece_type
    {
        pawn = 1,
        knight = 2,
        bishop = 3,
        rook = 4,
        queen = 5,
        king = 6
    };

    enum piece_colour
    {
        white = 0,
        black = 1 << 3
    };

    struct board_state
    {
        piece_colour current_player = white;
        
        // row - col
        uint8_t tiles[8][8];
        board_state()
        {
            std::fill(tiles[1], tiles[1] + 8, pawn | white);
            std::fill(tiles[6], tiles[6] + 8, pawn | black);

            uint8_t major_minor_pieces[8] = {
                rook, knight, bishop, queen, king, bishop, knight, rook
            };

            for (int idx = 0 ; idx < 8 ; idx++) {
                tiles[0][idx] = major_minor_pieces[idx] | white;
                tiles[7][idx] = major_minor_pieces[idx] | black;
            }
        }
    };
}