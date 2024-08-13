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

    return 0;
}
