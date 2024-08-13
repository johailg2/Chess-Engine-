#include "../include/eval.h"
#include <intrin.h>
#include <chrono>
using namespace std::chrono;

#define FLIP(sq) ((sq)^56)

#define mateValue 49000
#define mateScore 48000

int middleGamePieceValues[6] = { 150, 550, 400, 450, 1000, 0 }; // Increased Knight, Bishop, and Rook values for more activity
int endGamePieceValues[6] = { 120, 520, 330, 370, 950, 0 };

vector<int> centerSquares = {D4, D5, E4, E4};
vector<int> semiCenterSquares = {C3, C4, F3, F4, C5, C6, F5, F6};


const int middleGamePawnTable[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

const int middleGameRookTable[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

const int middleGameKnightTable[64] = {
    -105, -21, -58, -33, -17, -28, -19,  -23,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -13,   4,  16,  13,  28,  19,  21,   -8,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -47,  60,  37,  65,  84, 129,  73,   44,
     -73, -41,  72,  36,  23,  62,   7,  -17,
    -167, -89, -34, -49,  61, -97, -15, -107,
};

const int middleGameBishopTable[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

const int middleGameQueenTable[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

const int middleGameKingTable[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

const int endGamePawnTable[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

const int endGameRookTable[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

const int endGameKnightTable[64]  = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

const int endGameBishopTable[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

const int endGameQueenTable[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

const int endGameKingTable[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

const int isolatedPawnPenalty[2] = {-30, -25};
const int doublePawnPenalty[2] = {-25, -22};

const int middleGamepassedPawnBonus[8] = {0, 15, 30, 45, 60, 80, 120, 250};
const int endGamepassedPawnBonus[8] = {0, 30, 50, 70, 100, 150, 250, 300};

const int openFileBonus[2] = {30, 50}; 
const int semiOpenFileBonus[2] = {20, 40};


const int* middleGameTables[6] =
{
    middleGamePawnTable,
    middleGameRookTable,
    middleGameKnightTable,
    middleGameBishopTable,
    middleGameQueenTable,
    middleGameKingTable
};

const int* endGameTables[6] =
{
    endGamePawnTable,
    endGameRookTable,
    endGameKnightTable,
    endGameBishopTable,
    endGameQueenTable,
    endGameKingTable
};

int middleGameMVVLVA[6][6];
int endGameMVVLVA[6][6];

int middleGamePreComputedValues[2][6][64];
int endGamePreComputedValues[2][6][64];

Move pvTable[64][64];
int pvLength[64];

vector<vector<Move>> killerMoves(64, vector<Move>(2)); //max depth of 64, storing 2 moves per depth
vector<vector<vector<int>>> historyMoves(2, vector<vector<int>>(6, vector<int>(64))); //2 colors 6 pieces 64 squares

int Evaluation::evaluate(const ChessBoard& board)
{
    return evaluateMaterial(board, 1) + structuralEvaluation(board, 0) + evaluatePieceSquareTables(board, 1); + mobilityAndCenterControl(board, 1);
}

int Evaluation::evaluateMaterial(const ChessBoard &board, bool middleGame) 
{
    int score = 0;

    for(int pieceType = PieceType::PAWN; pieceType <= PieceType::KING; ++pieceType){
        auto whitePieceBoard = board.getPieceBoard(Piece(WHITE, static_cast<PieceType>(pieceType)));
        auto blackPieceBoard = board.getPieceBoard(Piece(BLACK, static_cast<PieceType>(pieceType)));

        int whitePieceCount = __builtin_popcountll(whitePieceBoard);
        int blackPieceCount = __builtin_popcountll(blackPieceBoard);

        score += whitePieceCount * (middleGame ? middleGamePieceValues[pieceType] : endGamePieceValues[pieceType]);
        score -= blackPieceCount * (middleGame ? middleGamePieceValues[pieceType] : endGamePieceValues[pieceType]);
    }

    return score;
}

void Evaluation::initializePieceSquareTables() 
{
    for (int pieceType = PieceType::PAWN; pieceType <= PieceType::KING; ++pieceType) {
        for (int square = 0; square < 64; ++square) {
            middleGamePreComputedValues[WHITE][pieceType][square] = middleGameTables[pieceType][square];
            middleGamePreComputedValues[BLACK][pieceType][square] = middleGameTables[pieceType][FLIP(square)];
            endGamePreComputedValues[WHITE][pieceType][square] = endGameTables[pieceType][square];
            endGamePreComputedValues[BLACK][pieceType][square] = endGameTables[pieceType][FLIP(square)];
        }
    }
}

int Evaluation::evaluatePieceSquareTables(const ChessBoard &board, bool isMiddleGame) 
{
    int score = 0;

    for (int pieceType = PieceType::PAWN; pieceType <= PieceType::KING; ++pieceType) {
        auto whitePieceBoard = BitBoard(board.getPieceBoard(Piece(WHITE, static_cast<PieceType>(pieceType))));
        auto blackPieceBoard = BitBoard(board.getPieceBoard(Piece(BLACK, static_cast<PieceType>(pieceType))));

        while (whitePieceBoard.getBoard()) {
            int square = whitePieceBoard.LS1B();
            whitePieceBoard.flipBit(square);
            // if (pieceType == PieceType::KNIGHT) {
            //     int knightScore = isMiddleGame ? middleGamePreComputedValues[WHITE][pieceType][63-square] 
            //                                    : endGamePreComputedValues[WHITE][pieceType][63-square];
            //     std::cout << "White knight on square " << square 
            //               << ", score: " << knightScore 
            //               << ", total score: " << score + knightScore << "\n";
            // }
            if (!isMiddleGame) {
                score += endGamePreComputedValues[WHITE][pieceType][63-square];
            } 
            else {
                score += middleGamePreComputedValues[WHITE][pieceType][63-square];
            }
        }

        while (blackPieceBoard.getBoard()) {
            int square = blackPieceBoard.LS1B();
            blackPieceBoard.flipBit(square);
            // if (pieceType == PieceType::KNIGHT) {
            //     int knightScore = isMiddleGame ? middleGamePreComputedValues[BLACK][pieceType][63-square] 
            //                                    : endGamePreComputedValues[BLACK][pieceType][63-square];
            //     std::cout << "Black knight on square " << square 
            //               << ", score: " << -knightScore 
            //               << ", total score: " << score - knightScore << "\n";
            // }
            if (!isMiddleGame) {
                score -= endGamePreComputedValues[BLACK][pieceType][63-square];
            } 

            else {
                score -= middleGamePreComputedValues[BLACK][pieceType][63-square];
            }
        }
    }

    return score;
}

int Evaluation::structuralEvaluation(const ChessBoard& board, bool isMiddleGame) 
{
    int score = 0;
    auto whitePawns = board.getPieceBoard(Piece(WHITE, PAWN));
    auto blackPawns = board.getPieceBoard(Piece(BLACK, PAWN));
    auto whiteRooks = board.getPieceBoard(Piece(WHITE, ROOK));
    auto blackRooks = board.getPieceBoard(Piece(BLACK, ROOK));
    auto whiteKing = board.getPieceBoard(Piece(WHITE, KING));
    auto blackKing = board.getPieceBoard(Piece(WHITE, KING));
    int whiteFilePawnCount[8] = {0};
    int blackFilePawnCount[8] = {0};

    int centralSquareBonus = 10;
    int semiCentralSquareBonus = 5;


    for (int file = 0; file < 8; file++) {
        auto currentFile = board.fileMasks[file];
        auto whitePawnsOnFile = currentFile & whitePawns;
        auto blackPawnsOnFile = currentFile & blackPawns;
        int whitePawnCount = __builtin_popcountll(whitePawnsOnFile);
        int blackPawnCount = __builtin_popcountll(blackPawnsOnFile);

        whiteFilePawnCount[file] = whitePawnCount;
        blackFilePawnCount[file] = blackPawnCount;  

        if (whitePawnCount > 1) {
            score += whitePawnCount * (isMiddleGame ? doublePawnPenalty[0] : doublePawnPenalty[1]);
        }

        if (blackPawnCount > 1) {
            score -= blackPawnCount * (isMiddleGame ? doublePawnPenalty[0] : doublePawnPenalty[1]);
        }

        bool isOpenFile = (whitePawnsOnFile == 0) && (blackPawnsOnFile == 0);
        bool isWhiteSemiOpenFile = (whitePawnsOnFile == 0) && (blackPawnsOnFile != 0);
        bool isBlackSemiOpenFile = (blackPawnsOnFile == 0) && (whitePawnsOnFile != 0);

        auto whiteRooksOnFile = currentFile & whiteRooks;
        auto blackRooksOnFile = currentFile & blackRooks;

        if(whiteRooksOnFile){
            if(isOpenFile){
                score += openFileBonus[isMiddleGame ? 0 : 1];
            }
            else if(isWhiteSemiOpenFile){
                score += semiOpenFileBonus[isMiddleGame ? 0 : 1];
            }
        }
        if(blackRooksOnFile){
            if(isOpenFile){
                score -= openFileBonus[isMiddleGame ? 0 : 1];
            }
            else if(isBlackSemiOpenFile){
                score -= semiOpenFileBonus[isMiddleGame ? 0 : 1];
            }
        }

        if(whiteKing & currentFile){
            if(isOpenFile){
                score -= openFileBonus[isMiddleGame ? 0 : 1] + 100;
            }
            else if(isWhiteSemiOpenFile){
                score -= semiOpenFileBonus[isMiddleGame ? 0 : 1] + 100;
            }
        }

        if(blackKing & currentFile){
            if(isOpenFile){
                score += openFileBonus[isMiddleGame ? 0 : 1] + 100;
            }
            else if(isBlackSemiOpenFile){
                score += semiOpenFileBonus[isMiddleGame ? 0 : 1] + 100;
            }
        }

    }

    auto whitePawnBoard = BitBoard(whitePawns);
    auto blackPawnBoard = BitBoard(blackPawns);

    for (int centralSquare : centerSquares) {
        if (whitePawnBoard.getBoard() & (1ULL << centralSquare)) {
            score += centralSquareBonus;
        }
    }

    for (int nearCentralSquare : semiCenterSquares) {
        if (whitePawnBoard.getBoard() & (1ULL << nearCentralSquare)) {
            score += semiCentralSquareBonus;
        }
    }

    for (int centralSquare : centerSquares) {
        if (blackPawnBoard.getBoard() & (1ULL << centralSquare)) {
            score -= centralSquareBonus;
        }
    }

    for (int nearCentralSquare : semiCenterSquares) {
        if (blackPawnBoard.getBoard() & (1ULL << nearCentralSquare)) {
            score -= semiCentralSquareBonus;
        }
    }

    while(whitePawnBoard.getBoard()) {
        auto square = whitePawnBoard.LS1B();
        whitePawnBoard.flipBit(square);
        if (!(board.isolatedMasks[square] & whitePawns)) {
            score += isolatedPawnPenalty[(isMiddleGame ? 0 : 1)];
        }
        if (!(board.whitePassedMasks[square] & blackPawns)) {
            int rank = square / 8;
            score +=  (isMiddleGame ? middleGamepassedPawnBonus[rank] : endGamepassedPawnBonus[rank]);
        }
    }

    while(blackPawnBoard.getBoard()) {
        auto square = blackPawnBoard.LS1B();
        blackPawnBoard.flipBit(square);
        if (!(board.isolatedMasks[square] & blackPawns)) {
            score -= isolatedPawnPenalty[(isMiddleGame ? 0 : 1)];
        }
        if (!(board.blackPassedMasks[square] & whitePawns)) {
            int rank = square / 8;
            score -=  (isMiddleGame ? middleGamepassedPawnBonus[7 - rank] : endGamepassedPawnBonus[7 - rank]);
        }
    }

    return score;
}
int Evaluation::mobilityAndCenterControl(const ChessBoard& board, bool isMiddleGame)
{
    int score = 0;
    auto allPieces = board.getAllPieces();
    auto whiteBishops = BitBoard(board.getPieceBoard(Piece(WHITE, BISHOP)));
    auto blackBishops = BitBoard(board.getPieceBoard(Piece(BLACK, BISHOP)));
    auto whiteQueen = BitBoard(board.getPieceBoard(Piece(WHITE, QUEEN)));
    auto blackQueen = BitBoard(board.getPieceBoard(Piece(BLACK, QUEEN)));
    auto whiteKnights = BitBoard(board.getPieceBoard(Piece(WHITE, KNIGHT)));
    auto blackKnights = BitBoard(board.getPieceBoard(Piece(WHITE, KNIGHT)));

    int bishopMobilityBonus = isMiddleGame ? 5 : 3;
    int queenMobilityBonus = isMiddleGame ? 3 : 1; 
    int centralSquareBonus = 10;
    int semiCentralSquareBonus = 5;

    while(whiteBishops.getBoard()){
        auto square = whiteBishops.LS1B();
        whiteBishops.flipBit(square);
        auto attackBoard = board.bishopRestrictedAttackMasks(square, allPieces);
        int range = __builtin_popcountll(attackBoard);
        score += range * bishopMobilityBonus;

        for (int centralSquare : centerSquares) {
            if (attackBoard & (1ULL << centralSquare)) {
                score += centralSquareBonus;
            }
        }
        for (int nearCentralSquare : semiCenterSquares) {
            if (attackBoard & (1ULL << nearCentralSquare)) {
                score += semiCentralSquareBonus;
            }
        }

    }
    
    while(blackBishops.getBoard()){
        auto square = blackBishops.LS1B();
        blackBishops.flipBit(square);
        auto attackBoard = board.bishopRestrictedAttackMasks(square, allPieces);
        int range = __builtin_popcountll(attackBoard);
        score -= range * bishopMobilityBonus;

        for (int centralSquare : centerSquares) {
            if (attackBoard & (1ULL << centralSquare)) {
                score += centralSquareBonus;
            }
        }
        for (int nearCentralSquare : semiCenterSquares) {
            if (attackBoard & (1ULL << nearCentralSquare)) {
                score += semiCentralSquareBonus;
            }
        }
    }

    while(whiteKnights.getBoard()){
        auto square = whiteKnights.LS1B();
        whiteKnights.flipBit(square);
        auto attackBoard = board.generateKnightAttacks(square) & board.getAllPieces();

        for (int centralSquare : centerSquares) {
            if (attackBoard & (1ULL << centralSquare)) {
                score += centralSquareBonus;
            }
        }
        for (int nearCentralSquare : semiCenterSquares) {
            if (attackBoard & (1ULL << nearCentralSquare)) {
                score += semiCentralSquareBonus;
            }
        }
    }    
    
    while(blackKnights.getBoard()){
        auto square = blackKnights.LS1B();
        blackKnights.flipBit(square);
        auto attackBoard = board.generateKnightAttacks(square) & board.getAllPieces();

        for (int centralSquare : centerSquares) {
            if (attackBoard & (1ULL << centralSquare)) {
                score -= centralSquareBonus;
            }
        }
        for (int nearCentralSquare : semiCenterSquares) {
            if (attackBoard & (1ULL << nearCentralSquare)) {
                score -= semiCentralSquareBonus;
            }
        }
    }

    while(whiteQueen.getBoard()){
        auto square = whiteQueen.LS1B();
        whiteQueen.flipBit(square);
        auto attackBoard = board.queenRestrictedAttackMasks(square, allPieces);
        int range = __builtin_popcountll(attackBoard);
        score += range * queenMobilityBonus;
    }

    while(blackQueen.getBoard()){
        auto square = blackQueen.LS1B();
        blackQueen.flipBit(square);
        auto attackBoard = board.queenRestrictedAttackMasks(square, allPieces);
        int range = __builtin_popcountll(attackBoard);
        score -= range * queenMobilityBonus; 
    }
    
    return score;
}


void Evaluation::initializeMVVLVA()
{
    for(int victim = PAWN; victim <= KING; victim++){
        for(int attacker = PAWN; attacker <= KING; attacker++){
            middleGameMVVLVA[victim][attacker] = middleGamePieceValues[victim] + 5000 - middleGamePieceValues[attacker];
            endGameMVVLVA[victim][attacker] = endGamePieceValues[victim] + 5000 - endGamePieceValues[attacker];
        }
    }
}

int Evaluation::quiescenceSearch(ChessBoard &board, int whitesGuaranteedBestMove, int blacksGuaranteedBestMove, bool player) 
{
    //evaluate at leaf nodes
    int leafNodeEval = evaluate(board);

    //if leaf node eval exceed alpha beta bounds update them
    if (player) { 
        if (leafNodeEval >= blacksGuaranteedBestMove) {
            return blacksGuaranteedBestMove;
        }
        if (whitesGuaranteedBestMove < leafNodeEval) {
            whitesGuaranteedBestMove = leafNodeEval; 
        }
    } 
    else { 
        if (leafNodeEval <= whitesGuaranteedBestMove) {
            return whitesGuaranteedBestMove;
        }
        if (blacksGuaranteedBestMove > leafNodeEval) {
            blacksGuaranteedBestMove = leafNodeEval; 
        }
    }

    //search through captures
    vector<Move> captureMoves;
    board.generateCaptureMoves(player ? WHITE : BLACK, captureMoves);

    for (auto& move : captureMoves) {
        board.saveBoardState();
        if (!board.makeAMove(move)) {
            continue;
        }
        int score;
        if (player) {
            score = quiescenceSearch(board, whitesGuaranteedBestMove, blacksGuaranteedBestMove, !player);
        } 
        
        else {
            score = quiescenceSearch(board, whitesGuaranteedBestMove, blacksGuaranteedBestMove, !player);
        }
        board.restoreBoardState();

        //update alpha beta bounds
        if (player) { 
            if (score >= blacksGuaranteedBestMove) {
                return blacksGuaranteedBestMove; 
            }
            if (score > whitesGuaranteedBestMove) {
                whitesGuaranteedBestMove = score; 
            }
        } 
        
        else { 
            if (score <= whitesGuaranteedBestMove) {
                return whitesGuaranteedBestMove; 
            }
            if (score < blacksGuaranteedBestMove) {
                blacksGuaranteedBestMove = score;
            }
        }
    }

    return player ? whitesGuaranteedBestMove : blacksGuaranteedBestMove;
}

SearchResult Evaluation::minMax(ChessBoard &board, int depth, int whitesGuaranteedBestMove, int blacksGuaranteedBestMove, bool player, vector<Move>& pv) 
{
    unsigned long long hash = board.currentHash;
    TranspositionTableEntry entry;

    if (depth == 0) {//check the leaf nodes and avoid horizon effect
        int eval = quiescenceSearch(board, whitesGuaranteedBestMove, blacksGuaranteedBestMove, player);
        return {eval, Move(), pv};
    }

    if(entryRetrieval(hash, entry, depth, whitesGuaranteedBestMove, blacksGuaranteedBestMove)){
        if (entry.evaluatedDepth >= depth) {
            if (entry.moveFlag == EXACT) {
                return {entry.positionScore, entry.bestMove, entry.pv};
            } 
            else if (entry.moveFlag == LOWER) {
                whitesGuaranteedBestMove = max(whitesGuaranteedBestMove, entry.positionScore);
            } 
            else if (entry.moveFlag == UPPER) {
                blacksGuaranteedBestMove = min(blacksGuaranteedBestMove, entry.positionScore);
            }
            if (whitesGuaranteedBestMove >= blacksGuaranteedBestMove) {
                return {entry.positionScore, entry.bestMove, entry.pv};
            }
        }
    }

    bool couldAMoveBeMade = false;
    SearchResult result;
    BoundFlag flag = LOWER;

    if (player) { // WHITE
        int maxEval = INT_MIN;
        Move bestMove;
        vector<Move> moveList;
        vector<Move> currentPV;
        board.generateMoves(WHITE, moveList);
        bool pvNodeFound = 0;
        int i = 0;

        sortMove(moveList, depth, pv);

        for (auto move : moveList) {
            i++;
            board.saveBoardState();
            if (!board.makeAMove(move)) {
                continue;
            }
            couldAMoveBeMade = true;

            if (pvNodeFound) { //if a PV node is found, search it with a null window, to essentially "prove" that it is the best move
                result = minMax(board, depth - 1, whitesGuaranteedBestMove, whitesGuaranteedBestMove + 1, !player, currentPV);
                if (result.evaluation > whitesGuaranteedBestMove && result.evaluation < blacksGuaranteedBestMove) { //if it turns out that it isn't the best move, run a full search on it
                    result = minMax(board, depth - 1, whitesGuaranteedBestMove, blacksGuaranteedBestMove, !player, currentPV);
                }
            } else { //regular moves get a regular search
                int reduction = (i > 0 && depth > 2) ? 2 : 0; //apply LMR for non-PV moves
                result = minMax(board, depth - 1 - reduction, whitesGuaranteedBestMove, blacksGuaranteedBestMove, !player, currentPV);
            }

            board.restoreBoardState();

            if (result.evaluation > maxEval) { //found the best move in the position
                maxEval = result.evaluation;
                bestMove = move;
                pv.clear();
                pv.push_back(bestMove);
                pv.insert(pv.end(), result.pv.begin(), result.pv.end()); //update the PV
                pvNodeFound = 1; //found a PV node
                flag = EXACT;  
            }

            whitesGuaranteedBestMove = max(whitesGuaranteedBestMove, maxEval);  //update alpha bound
            if (blacksGuaranteedBestMove <= whitesGuaranteedBestMove) {  //Beta cutoff: prune the remaining branches as Black will not allow this move
                updateHistoryMoves(depth, move); //update history and killer moves on cutoff
                updateKillerMoves(depth, move);
                flag = LOWER;
                break;
            }
        }
        if (couldAMoveBeMade) {
            flag = (maxEval >= blacksGuaranteedBestMove) ? LOWER : ((maxEval <= whitesGuaranteedBestMove) ? UPPER : EXACT);
            entry = {hash, depth, maxEval, pv, bestMove, flag};
            insertToTable(hash, entry);            
            return {maxEval, bestMove, pv};
        } else {
            int kingSquare = BitBoard(board.getPieceBoard(Piece(WHITE, KING))).LS1B();
            bool inCheck = board.isSquareAttacked(kingSquare, BLACK);
            if (inCheck) {
                entry = {hash, depth, -mateValue, pv, Move(), EXACT};
                insertToTable(hash, entry);
                return {-mateValue + depth, Move(), pv}; //checkmate
            } else {
                entry = {hash, depth, 0, pv, Move(), EXACT};
                insertToTable(hash, entry);
                return {0, Move(), pv}; //stalemate
            }
        }
    } else { // BLACK
        int minEval = INT_MAX;
        Move bestMove;
        vector<Move> moveList;
        vector<Move> currentPV;
        board.generateMoves(BLACK, moveList);
        bool pvNodeFound = false;
        int i;

        sortMove(moveList, depth, pv);

        for (auto move : moveList) {
            i++;
            board.saveBoardState();
            if (!board.makeAMove(move)) {
                continue;
            }
            couldAMoveBeMade = true;

            if (pvNodeFound) {
                result = minMax(board, depth - 1, blacksGuaranteedBestMove - 1, blacksGuaranteedBestMove, !player, currentPV);
                if (result.evaluation > whitesGuaranteedBestMove && result.evaluation < blacksGuaranteedBestMove) {
                    result = minMax(board, depth - 1, whitesGuaranteedBestMove, blacksGuaranteedBestMove, !player, currentPV);
                }
            } else {
                int reduction = (i > 0 && depth > 2) ? 2 : 0;
                result = minMax(board, depth - 1 - reduction, whitesGuaranteedBestMove, blacksGuaranteedBestMove, !player, currentPV);
            }

            board.restoreBoardState();

            if (result.evaluation < minEval) {
                minEval = result.evaluation;
                bestMove = move;
                pv.clear();
                pv.push_back(bestMove);
                pv.insert(pv.end(), result.pv.begin(), result.pv.end());
                pvNodeFound = 1;
                flag = EXACT;
            }

            blacksGuaranteedBestMove = min(blacksGuaranteedBestMove, minEval); //update beta bound
            if (blacksGuaranteedBestMove <= whitesGuaranteedBestMove) { //Alpha cutoff: prune the remaining branches as White will not allow this move
                flag = UPPER;
                updateHistoryMoves(depth, move); //update history and killer moves on cutoff
                updateKillerMoves(depth, move);
                break;
            }
        }
        if (couldAMoveBeMade) {
            flag = (minEval <= whitesGuaranteedBestMove) ? UPPER : ((minEval >= blacksGuaranteedBestMove) ? LOWER : EXACT);
            entry = {hash, depth, minEval, pv, bestMove, flag};
            insertToTable(hash, entry);
            return {minEval, bestMove, pv};
        } else {
            int kingSquare = BitBoard(board.getPieceBoard(Piece(BLACK, KING))).LS1B();
            bool inCheck = board.isSquareAttacked(kingSquare, WHITE);
            if (inCheck) {
                entry = {hash, depth, mateValue, pv, Move(), EXACT};
                insertToTable(hash, entry);
                return {mateValue - depth, Move(), pv}; 
            } else {
                entry = {hash, depth, 0, pv, Move(), EXACT};
                insertToTable(hash, entry);
                return {0, Move(), pv}; 
            }
        }
    }
}

SearchResult Evaluation::iterativeDeepeningSearch(ChessBoard &board, int depth, bool player) {
    SearchResult finalResult;
    vector<Move> pv;

    auto start = high_resolution_clock::now();
    int aspirationWindow = 50;

    int previousEval = 0; 

    for (int currentDepth = 1; currentDepth <= depth; ++currentDepth) {
        cout << "Starting search at depth: " << currentDepth << endl;
        int alpha = previousEval - aspirationWindow;
        int beta = previousEval + aspirationWindow;

        while (true) {
            finalResult = minMax(board, currentDepth, alpha, beta, player, pv);
            if (finalResult.evaluation <= alpha) {
                alpha = INT_MIN;
            } else if (finalResult.evaluation >= beta) {
                beta = INT_MAX;
            } else {
                break;
            }
        }

        previousEval = finalResult.evaluation;

        cout << "Depth: " << currentDepth << " Best Move: ";
        board.printMove(finalResult.bestMove.startingSquare, finalResult.bestMove.endSquare);
        cout << " Evaluation: " << finalResult.evaluation << endl;
    }

    auto stop = high_resolution_clock::now(); // Stop timing
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Principal Variation: ";
    for (const Move& move : pv) {
        board.printMove(move.startingSquare, move.endSquare);
        cout << " " << endl;
    }

    cout << "Time taken for search: " << duration.count() << " milliseconds" << endl;

    return finalResult;
}


int Evaluation::scoreMove(Move& move, int depth, const vector<Move>& pv) {
    if (pv.size() >= depth && move == pv[depth - 1]) {
        return 20000;
    }

    if (move == killerMoves[depth][0]) {
        return 10000;
    } else if (move == killerMoves[depth][1]) {
        return 7500;
    }

    int victim = move.capturedPiece.type;
    int attacker = move.movingPieceType;
    int historyScore = historyMoves[move.movingPieceColor][move.movingPieceType][move.endSquare];

    int mvvLvaScore = middleGame ? middleGameMVVLVA[victim][attacker] : endGameMVVLVA[victim][attacker];
    return mvvLvaScore + historyScore;
}

inline void Evaluation::sortMove(std::vector<Move>& moveList, int depth, const std::vector<Move>& pv) {
    std::sort(moveList.begin(), moveList.end(), [this, depth, &pv](Move& move1, Move& move2) {
        return scoreMove(move1, depth, pv) > scoreMove(move2, depth, pv);
    });
}

void Evaluation::updateKillerMoves(int depth, const Move& move) {
    if (move.capturedPiece.type == EMPTY) {
        if (!(killerMoves[depth][0] == move) && !(killerMoves[depth][1] == move)) {
            killerMoves[depth][1] = killerMoves[depth][0];
            killerMoves[depth][0] = move;
        }
    }
}

void Evaluation::updateHistoryMoves(int depth, const Move& move) {
    if (move.capturedPiece.type == EMPTY) {
        historyMoves[move.movingPieceColor][move.movingPieceType][move.endSquare] += depth * depth;
    }
}

void updatePV(vector<Move>& pv, const vector<Move>& childPV, const Move& move) {
    pv.clear();
    pv.push_back(move);
    pv.insert(pv.end(), childPV.begin(), childPV.end());
}

void Evaluation::initializeKillerMoves() 
{
    for (auto& movesAtDepth : killerMoves) {
        movesAtDepth[0] = Move(-1, -1, EMPTY, WHITE, Piece(), EMPTY, false, false, false, false);
        movesAtDepth[1] = Move(-1, -1, EMPTY, WHITE, Piece(), EMPTY, false, false, false, false);
    }
}

void Evaluation::initializeHistoryMoves()
{
    for(auto& color : historyMoves){
        for(auto& pieces : color){
            fill(pieces.begin(), pieces.end(), 0);
        }
    }
}

void Evaluation::clearTranspositionTable()
{
    transpositionTable.clear();
}

void Evaluation::insertToTable(unsigned long long hash, TranspositionTableEntry &entry) 
{
    auto &entriesForHash = transpositionTable[hash];
    if(entry.positionScore < -mateScore){
        entry.positionScore -= entry.evaluatedDepth;
    }
    if(entry.positionScore > mateScore){
        entry.positionScore += entry.evaluatedDepth;
    }
    auto it = std::find_if(entriesForHash.begin(), entriesForHash.end(), [&](const TranspositionTableEntry &e) {
        return e.hash == hash;
    });

    if (it != entriesForHash.end()) {
        if (entry.evaluatedDepth > it->evaluatedDepth || 
            (entry.evaluatedDepth == it->evaluatedDepth && entry.moveFlag == EXACT)) {
            *it = entry;
        }
    } else {
        entriesForHash.push_back(entry); 
    }
}


bool Evaluation::entryRetrieval(unsigned long long hash, TranspositionTableEntry &entry, int depth, int alpha, int beta) const 
{
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end()) {
        for (const auto &e : it->second) {
            if (e.evaluatedDepth >= depth) {
                entry = e;
                if(entry.positionScore > mateScore){
                    entry.positionScore -= entry.evaluatedDepth;
                }
                if(entry.positionScore < mateScore){
                    entry.positionScore += entry.evaluatedDepth;
                }
                return 1;
            }
        }
    }
    return 0;
}
