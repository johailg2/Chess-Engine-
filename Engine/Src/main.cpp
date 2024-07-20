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
    Evaluation eval(5);
    ChessBoard board;

    // Initialize the board with a specific FEN
    // This position is set up for a more populated scenario with promotion possibilities
        board.setBoardFromFEN("4nn1r/1prbk1pq/2pp1p2/4pP1P/4P1QQ/3B2PP/1NRP1P2/1NKRB3 b - - 0 1");
        board.printChessBoard();
        board.initialize();

    board.printChessBoard();
    board.initialize();

    // Print the initial board state
    std::cout << "Initial board state:" << std::endl;
    board.printChessBoard();

    // Generate a sequence of best moves
    int maxDepth = 5; // Depth for iterative deepening search
    int numMoves = 10; // Number of moves to generate

    // Test Zobrist hashing consistency
    printBestMoveSequence(eval, board, 4, 10);

    return 0;
}

void printBestMoveSequence(Evaluation& eval, ChessBoard& board, int maxDepth, int numMoves) {
    bool player = false; // true for White, false for Black
    std::vector<unsigned long long> hashHistory;

    for (int moveCount = 0; moveCount < numMoves; ++moveCount) {
        // Save the current board state and hash key
        board.saveBoardState();
        unsigned long long oldHash = board.currentHash;
        hashHistory.push_back(oldHash);

        std::cout << "Move " << moveCount + 1 << ": Current player is " << (player ? "White" : "Black") << std::endl;

        SearchResult result = eval.iterativeDeepeningSearch(board, maxDepth, player);

        // Print the move
        std::cout << "Best move: ";
        board.printMove(result.bestMove.startingSquare, result.bestMove.endSquare);
        std::cout << std::endl;

        // Print the board state before the move
        std::cout << "Board state before move:\n";
        board.printChessBoard();

        // Log the current hash before making the move
        std::cout << "Old hash: 0x" << std::hex << oldHash << std::dec << std::endl;

        // Make the move
        if (!board.makeAMove(result.bestMove)) {
            std::cerr << "Failed to make the move." << std::endl;
            break;
        }

        // Print the updated board
        std::cout << "Board state after move:\n";
        board.printChessBoard();

        // Get the new hash
        unsigned long long newHash = board.currentHash;

        // Print the new hash after making the move
        std::cout << "New hash: 0x" << std::hex << newHash << std::dec << std::endl;

        // Validate hash key consistency by generating the expected hash
        unsigned long long expectedHash = board.generateInitialHash();
        std::cout << "Expected hash after generating: 0x" << std::hex << expectedHash << std::dec << std::endl;

        if (newHash != expectedHash) {
            std::cerr << "Hash key mismatch after making the move!" << std::endl;
            std::cerr << "Expected: 0x" << std::hex << expectedHash << std::endl;
            std::cerr << "Found: 0x" << std::hex << newHash << std::dec << std::endl;
            break;
        }

        player = !player; // Switch player
    }

    // Undo moves and check hash consistency
    while (!hashHistory.empty()) {
        unsigned long long expectedHash = hashHistory.back();
        hashHistory.pop_back();

        board.restoreBoardState();

        unsigned long long currentHash = board.currentHash;
        if (currentHash != expectedHash) {
            std::cerr << "Hash key mismatch after undoing the move!" << std::endl;
            std::cerr << "Expected: 0x" << std::hex << expectedHash << std::endl;
            std::cerr << "Found: 0x" << std::hex << currentHash << std::dec << std::endl;
            break;
        }

        // Print the reverted board
        board.printChessBoard();
    }
}
