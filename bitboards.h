#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <stdio.h>

class ChessBoard;

class BitBoard {
public:
    BitBoard();

    BitBoard(std::string binary);

    BitBoard(unsigned long long board);

    inline int getBit(int square) const {
        return (bitBoard >> square) & 1ULL;
    }

    inline void setBit(int square) {
        bitBoard |= (1ULL << square);
    }

    inline void flipBit(int square) {
        bitBoard ^= (1ULL << square);
    }

    inline unsigned long long getBoard() const {
        return bitBoard;
    }

    inline void setBoard(unsigned long long board) {
        bitBoard = board;
    }

    void printBitBoard() const {
        int c = 0;
        for(int i = 63; i >= 0; i--){
            std::cout<<(getBit(i) ? "1" : "0")<< "  ";
            c++;
            if(c == 8){
                std::cout<<"\n";
                c = 0;
            }
        }
        std::cout<<"A1 B1 C1 D1 E1 F1 G1 H1";
        std::cout<<"\n";
    }


    inline int popCount() const {
        return __builtin_popcountll(bitBoard);
    }

    inline int LS1B() const {
        return __builtin_ffsll(bitBoard) - 1;
    }

private:
    unsigned long long bitBoard;
};

#endif // BITBOARDS_H
