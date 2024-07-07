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

#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <vector>


int main() {
Evaluation evaluator;
ChessBoard board;
board.initialize();
// Initialize the board with the current state
int depth = 5; // Example search depth
SearchResult result = evaluator.search(board, depth, INT_MIN, INT_MAX, true); // Assuming white to move
Move bestMove = result.bestMove;
std::cout << "Best Move: ";
board.printMove(bestMove.startingSquare, bestMove.endSquare);
std::cout <<" with evaluation: " << result.evaluation << std::endl;

    return 0;
}
