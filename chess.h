#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <stdint.h>

namespace chess
{
    // 000 - nothing
    // 001 - pawn
    // 010 - knight
    // 011 - bishop
    // 100 - rook
    // 101 - queen
    // 110 - king
    enum piece {
        pawn = 1,
        knight = 2,
        bishop = 3,
        rook = 4,
        queen = 5,
        king = 6
    };

    // 0000 - white
    // 1000 - black
    // e.g. 1110 - black king
    enum colour {
        white = 0,
        black = 1 << 3
    };

    class board
    {
        public:
            colour m_current_player = white;

            uint8_t m_tiles[64] = {
                white | rook, white | knight, white | bishop, white | queen, white | king, white | bishop, white | knight, white | rook,
                white | pawn, white | pawn, white | pawn, white | pawn, white | pawn, white | pawn, white | pawn, white | pawn,
                0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,
                black | pawn, black | pawn, black | pawn, black | pawn, black | pawn, black | pawn, black | pawn, black | pawn,
                black | rook, black | knight, black | bishop, black | queen, black | king, black | bishop, black | knight, black | rook,
            };

            uint8_t get_square(int x, int y) {
                int index = x + y*8;
                return m_tiles[index];
            }

            void set_square(int x, int y, uint8_t value) {
                int index = x + y*8;
                m_tiles[index] = value;
            }

            void print_board() {
                for (int y = 0 ; y < 8 ; y++) {
                    if (y == 0) {
                        std::cout << " a b c d e f g h \n";
                    }
                    for (int x = 0 ; x < 8 ; x++) {
                        if (x == 0) {
                            std::cout << y;
                        }    
                        uint8_t tile_value = get_square(x,y);
                        bool is_black = tile_value & black;
                        switch (tile_value & 0b111) {
                            case pawn: {
                                std::cout << (is_black ? "♟︎ " : "♙ ");
                            } break;

                            case rook: {
                                std::cout << (is_black ? "♜ " : "♖ ");
                            } break;

                            case knight: {
                                std::cout << (is_black ? "♞ " : "♘ ");
                            } break;

                            case bishop: {
                                std::cout << (is_black ? "♝ " : "♗ ");
                            } break;

                            case queen: {
                                std::cout << (is_black ? "♛ " : "♕ ");
                            } break;

                            case king: {
                                std::cout << (is_black ? "♚ " : "♔ ");
                            } break;

                            default: {
                                std::cout << "  ";
                            } break;
                        }
                    }
                    std::cout << std::endl;
                }
            }

            bool move_piece(int start_x, int start_y, int end_x, int end_y) {
                auto selected_square = get_square(start_x, start_y);
                if (selected_square) {
                    if (selected_square & m_current_player) {
                        set_square(end_x, end_y, selected_square);
                        set_square(start_x, start_y, 0);
                    }
                    else {
                        return false;
                    }
                }
                return false;
            }
    };
}