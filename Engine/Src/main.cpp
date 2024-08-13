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

void printBestMoveSequence(Evaluation& eval, ChessBoard& board, int maxDepth, int numMoves);
int main() {
    Evaluation eval; // Initialize Evaluation with depth of 5
    ChessBoard board;
    board.setBoardFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    board.initialize();
    board.printChessBoard();
    //eval.evaluate(board);
    eval.iterativeDeepeningSearch(board, 5, 1);

    // // Initialize the board with a specific FEN where checkmate is imminent
    // board.setBoardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // White to move and checkmate in 1
    // board.initialize();
    // SearchResult result = eval.iterativeDeepeningSearch(board, 7, 1);
    // board.printMove(result.pv[0].startingSquare, result.pv[0].endSquare);
    // std::cout << " Evaluation: " << result.evaluation << std::endl;
    // board.printChessBoard();

    return 0;
}

// void printBestMoveSequence(Evaluation& eval, ChessBoard& board, int maxDepth, int numMoves) {
//     bool player = false; // true for White, false for Black
//     std::vector<unsigned long long> hashHistory;

//     for (int moveCount = 0; moveCount < numMoves; ++moveCount) {
//         // Save the current board state and hash key
//         board.saveBoardState();
//         unsigned long long oldHash = board.currentHash;
//         hashHistory.push_back(oldHash);

//         std::cout << "Move " << moveCount + 1 << ": Current player is " << (player ? "White" : "Black") << std::endl;

//         SearchResult result = eval.iterativeDeepeningSearch(board, maxDepth, player);

//         // Print the move
//         std::cout << "Best move: ";
//         board.printMove(result.bestMove.startingSquare, result.bestMove.endSquare);
//         std::cout << std::endl;

//         // Print the board state before the move
//         std::cout << "Board state before move:\n";
//         board.printChessBoard();

//         // Log the current hash before making the move
//         std::cout << "Old hash: 0x" << std::hex << oldHash << std::dec << std::endl;

//         // Make the move
//         if (!board.makeAMove(result.bestMove)) {
//             std::cerr << "Failed to make the move." << std::endl;
//             break;
//         }

//         // Print the updated board
//         std::cout << "Board state after move:\n";
//         board.printChessBoard();

//         // Get the new hash
//         unsigned long long newHash = board.currentHash;

//         // Print the new hash after making the move
//         std::cout << "New hash: 0x" << std::hex << newHash << std::dec << std::endl;

//         // Validate hash key consistency by generating the expected hash
//         unsigned long long expectedHash = board.generateInitialHash();
//         std::cout << "Expected hash after generating: 0x" << std::hex << expectedHash << std::dec << std::endl;

//         if (newHash != expectedHash) {
//             std::cerr << "Hash key mismatch after making the move!" << std::endl;
//             std::cerr << "Expected: 0x" << std::hex << expectedHash << std::endl;
//             std::cerr << "Found: 0x" << std::hex << newHash << std::dec << std::endl;
//             break;
//         }

//         player = !player; // Switch player
//     }

//     // Undo moves and check hash consistency
//     while (!hashHistory.empty()) {
//         unsigned long long expectedHash = hashHistory.back();
//         hashHistory.pop_back();

//         board.restoreBoardState();

//         unsigned long long currentHash = board.currentHash;
//         if (currentHash != expectedHash) {
//             std::cerr << "Hash key mismatch after undoing the move!" << std::endl;
//             std::cerr << "Expected: 0x" << std::hex << expectedHash << std::endl;
//             std::cerr << "Found: 0x" << std::hex << currentHash << std::dec << std::endl;
//             break;
//         }

//         // Print the reverted board
//         board.printChessBoard();
//     }
// }
