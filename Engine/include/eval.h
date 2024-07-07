#include "../include/chessboard.h"
#include "../include/attackTables.h"
#include "../include/bitboards.h"

// all piece table values were found on the chess programming wiki site
extern const int middleGamePawnTable[64];
extern const int middleGameRookTable[64];
extern const int middleGameKnightTable[64];
extern const int middleGameBishopTable[64];
extern const int middleGameQueenTable[64];
extern const int middleGameKingTable[64];

extern const int endGamePawnTable[64];
extern const int endGameRookTable[64];
extern const int endGameKnightTable[64];
extern const int endGameBishopTable[64];
extern const int endGameQueenTable[64];
extern const int endGameKingTable[64];

struct SearchResult {
    int evaluation;
    Move bestMove;
};

class Evaluation{

public:
    static int evaluate(const ChessBoard& chessBoard);
    SearchResult search(ChessBoard& board, int depth, int bestEvaluatedWhiteScore, int bestEvaluatedBlackScore, bool player);


private:
    static int evaluateMaterial(const ChessBoard& board, bool gamePhase);
    static void initializePieceSquareTables();
    static int evaluatePieceSquareTables(const ChessBoard& board, bool gamePhase);
    static int evaluateMobility(const ChessBoard& board);
    static int evaluateKingSafety(const ChessBoard& board);
    static int evaluatePawnStructure(const ChessBoard& board);
};