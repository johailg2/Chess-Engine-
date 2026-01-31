#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <stdio.h>

// windows intrinsics
#ifdef _MSC_VER
#include <intrin.h>
#define __builtin_popcountll __popcnt64
#define __builtin_ffsll(x) (_tzcnt_u64(x) + 1) 
#endif

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
        for (int i = 63; i >= 0; i--) {
            std::cout << (getBit(i) ? "1" : "0") << "  ";
            c++;
            if (c == 8) {
                std::cout << "\n";
                c = 0;
            }
        }
        std::cout << "A1 B1 C1 D1 E1 F1 G1 H1";
        std::cout << "\n";
    }

    inline int popCount() const {
        #ifdef _MSC_VER
            return (int)__popcnt64(bitBoard);
        #else
            return __builtin_popcountll(bitBoard);
        #endif
    }

    inline int LS1B() const {
        #ifdef _MSC_VER
            unsigned long index;
            // _BitScanForward64 returns 0 if zero, non-zero if found
            if (_BitScanForward64(&index, bitBoard)) {
                return (int)index;
            }
            return -1; 
        #else
            return __builtin_ffsll(bitBoard) - 1;
        #endif
    }

private:
    unsigned long long bitBoard;
};

#endif // BITBOARDS_H