#include "../include/eval.h"
#include "../include/chessboard.h"
#include "../include/bitboards.h"
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <cstdlib>
#include <ctime>
#include "../include/attackTables.h"
#include "../include/eval.h"

// void printBestMoveSequence(Evaluation& eval, ChessBoard& board, int maxDepth, int numMoves) {
//     bool player = true; // true for White, false for Black
//     std::vector<unsigned long long> hashHistory;

//     for (int moveCount = 0; moveCount < numMoves; ++moveCount) {
//         // Store the current hash key
//         unsigned long long currentHash = board.generateHashForPosition();
//         hashHistory.push_back(currentHash);

//         SearchResult result = eval.iterativeDeepeningSearch(board, maxDepth, player);

//         // Print the updated board (optional)
//         board.printChessBoard();

//         // Validate hash key consistency
//         unsigned long long newHash = board.generateHashForPosition();
//         if (newHash != board.hashKey) {
//             std::cerr << "Hash key mismatch after making the move!" << std::endl;
//             std::cerr << "Expected: " << std::bitset<64>(newHash) << std::endl;
//             std::cerr << "Found: " << std::bitset<64>(board.hashKey) << std::endl;
//             break;
//         }

//         player = !player; // Switch player
//     }

//     // Undo moves and check hash consistency
//     while (!hashHistory.empty()) {
//         unsigned long long expectedHash = hashHistory.back();
//         hashHistory.pop_back();

//         board.undoMove();

//         unsigned long long currentHash = board.generateHashForPosition();
//         if (currentHash != expectedHash) {
//             std::cerr << "Hash key mismatch after undoing the move!" << std::endl;
//             std::cerr << "Expected: " << std::bitset<64>(expectedHash) << std::endl;
//             std::cerr << "Found: " << std::bitset<64>(currentHash) << std::endl;
//             break;
//         }

//         // Print the reverted board (optional)
//         board.printChessBoard();
//     }
// }

int main() {
    Evaluation eval(5);
    ChessBoard board;

    // Initialize the board with a specific FEN
    board.setBoardFromFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    board.initialize();

    // Print the initial board state
    std::cout << "Initial board state:" << std::endl;
    board.printChessBoard(); // Ensure this method is implemented in ChessBoard class

    // Generate a sequence of best moves
    int maxDepth = 3; // Depth for iterative deepening search
    int numMoves = 10; // Number of moves to generate

    // Test Zobrist hashing consistency
    eval.iterativeDeepeningSearch(board, 6, 1);
    return 0;
}


