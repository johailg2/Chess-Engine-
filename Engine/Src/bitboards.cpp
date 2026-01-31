#include "../headers/bitboards.h"
#include "../headers/chessboard.h"

BitBoard::BitBoard() : bitBoard(0ULL) {}

BitBoard::BitBoard(std::string binary) {
    bitBoard = std::bitset<64>(binary).to_ullong();
}

BitBoard::BitBoard(unsigned long long board) : bitBoard(board) {}
