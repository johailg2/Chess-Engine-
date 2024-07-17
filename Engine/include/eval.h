#ifndef EVAL_H
#define EVAL_H


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
    vector<Move> pv;
};

class Evaluation{

public:
    Evaluation(int max){
        initializeHistoryMoves();
        initializeKillerMoves();
        initializePieceSquareTables();
        initializeMVVLVA();
        maxDepth = max;
    };
    static int evaluate(const ChessBoard& chessBoard);
    static void initializeMVVLVA();
    int scoreMove(Move& move, int depth, const vector<Move>& pv);
    SearchResult iterativeDeepeningSearch(ChessBoard& board, int depth, bool player);
    void printPVTable(int depth);
    SearchResult minMax(ChessBoard& board, int depth, int bestEvaluatedWhiteScore, int bestEvaluatedBlackScore, bool player, vector<Move>& pv);


private:
    bool middleGame = true;
    int maxDepth = 64;
    static int evaluateMaterial(const ChessBoard& board, bool gamePhase);
    static void initializePieceSquareTables();
    static int evaluatePieceSquareTables(const ChessBoard& board, bool gamePhase);
    static int evaluateMobility(const ChessBoard& board);
    static int evaluateKingSafety(const ChessBoard& board);
    static int evaluatePawnStructure(const ChessBoard& board);
    int quiescenceSearch(ChessBoard &board, int alpha, int beta, bool player);
    void sortMove(vector<Move>& moveList, int depth,const vector<Move>& pv);
    void initializeKillerMoves();
    void initializeHistoryMoves();
    void updateKillerMoves(int depth, const Move& move);
    void updateHistoryMoves(int depth, const Move& move);
    void initializePVTable();
    void updatePVTable(int depth, const Move& move);
};


#endif // EVAL_H
