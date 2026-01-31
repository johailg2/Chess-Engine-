#include "../headers/eval.h"
#include <chrono>
#include <algorithm>

using namespace std::chrono;
using std::cout;
using std::endl;
using std::vector;
using std::max;
using std::min;

#define FLIP(sq) ((sq) ^ 56)
#define MATE_VALUE 49000
#define MATE_SCORE 48000

// ============================================================================
// PIECE VALUES
// ============================================================================

int middleGamePieceValues[6] = { 150, 550, 400, 450, 1000, 0 };
int endGamePieceValues[6]    = { 120, 520, 330, 370, 950, 0 };

vector<int> centerSquares = {D4, D5, E4, E5};
vector<int> semiCenterSquares = {C3, C4, F3, F4, C5, C6, F5, F6};

// ============================================================================
// PIECE SQUARE TABLES
// ============================================================================

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

const int endGameKnightTable[64] = {
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

// ============================================================================
// PAWN STRUCTURE BONUSES
// ============================================================================

const int isolatedPawnPenalty[2] = {-30, -25};
const int doublePawnPenalty[2] = {-25, -22};

const int middleGamePassedPawnBonus[8] = {0, 15, 30, 45, 60, 80, 120, 250};
const int endGamePassedPawnBonus[8] = {0, 30, 50, 70, 100, 150, 250, 300};

const int openFileBonus[2] = {30, 50};
const int semiOpenFileBonus[2] = {20, 40};

// ============================================================================
// TABLE POINTERS
// ============================================================================

const int* middleGameTables[6] = {
    middleGamePawnTable, middleGameRookTable, middleGameKnightTable,
    middleGameBishopTable, middleGameQueenTable, middleGameKingTable
};

const int* endGameTables[6] = {
    endGamePawnTable, endGameRookTable, endGameKnightTable,
    endGameBishopTable, endGameQueenTable, endGameKingTable
};

// ============================================================================
// MOVE ORDERING TABLES
// ============================================================================

int middleGameMVVLVA[6][6];
int endGameMVVLVA[6][6];

int middleGamePreComputedValues[2][6][64];
int endGamePreComputedValues[2][6][64];

Move pvTable[64][64];
int pvLength[64];

vector<vector<Move>> killerMoves(64, vector<Move>(2));
vector<vector<vector<int>>> historyMoves(2, vector<vector<int>>(6, vector<int>(64)));

// ============================================================================
// MAIN EVALUATION
// ============================================================================

int Evaluation::evaluate(const ChessBoard& board) {
    int material = evaluateMaterial(board, true);
    int structure = structuralEvaluation(board, false);
    int pst = evaluatePieceSquareTables(board, true);
    int mobility = mobilityAndCenterControl(board, true);
    
    return material + structure + pst + mobility;
}

int Evaluation::evaluateMaterial(const ChessBoard& board, bool middleGame) {
    int score = 0;
    const int* values = middleGame ? middleGamePieceValues : endGamePieceValues;
    
    for (int pieceType = PAWN; pieceType <= KING; pieceType++) {
        auto whitePieces = board.getPieceBoard(Piece(WHITE, static_cast<PieceType>(pieceType)));
        auto blackPieces = board.getPieceBoard(Piece(BLACK, static_cast<PieceType>(pieceType)));
        
        int whiteCount = __builtin_popcountll(whitePieces);
        int blackCount = __builtin_popcountll(blackPieces);
        
        score += (whiteCount - blackCount) * values[pieceType];
    }
    
    return score;
}

// ============================================================================
// PIECE SQUARE TABLE EVALUATION
// ============================================================================

void Evaluation::initializePieceSquareTables() {
    for (int pieceType = PAWN; pieceType <= KING; pieceType++) {
        for (int sq = 0; sq < 64; sq++) {
            middleGamePreComputedValues[WHITE][pieceType][sq] = middleGameTables[pieceType][sq];
            middleGamePreComputedValues[BLACK][pieceType][sq] = middleGameTables[pieceType][FLIP(sq)];
            endGamePreComputedValues[WHITE][pieceType][sq] = endGameTables[pieceType][sq];
            endGamePreComputedValues[BLACK][pieceType][sq] = endGameTables[pieceType][FLIP(sq)];
        }
    }
}

int Evaluation::evaluatePieceSquareTables(const ChessBoard& board, bool isMiddleGame) {
    int score = 0;
    
    for (int pieceType = PAWN; pieceType <= KING; pieceType++) {
        BitBoard whitePieces(board.getPieceBoard(Piece(WHITE, static_cast<PieceType>(pieceType))));
        BitBoard blackPieces(board.getPieceBoard(Piece(BLACK, static_cast<PieceType>(pieceType))));
        
        while (whitePieces.getBoard()) {
            int sq = whitePieces.LS1B();
            whitePieces.flipBit(sq);
            
            if (isMiddleGame) {
                score += middleGamePreComputedValues[WHITE][pieceType][63 - sq];
            } else {
                score += endGamePreComputedValues[WHITE][pieceType][63 - sq];
            }
        }
        
        while (blackPieces.getBoard()) {
            int sq = blackPieces.LS1B();
            blackPieces.flipBit(sq);
            
            if (isMiddleGame) {
                score -= middleGamePreComputedValues[BLACK][pieceType][63 - sq];
            } else {
                score -= endGamePreComputedValues[BLACK][pieceType][63 - sq];
            }
        }
    }
    
    return score;
}

// ============================================================================
// PAWN STRUCTURE EVALUATION
// ============================================================================

int Evaluation::structuralEvaluation(const ChessBoard& board, bool isMiddleGame) {
    int score = 0;
    int phaseIdx = isMiddleGame ? 0 : 1;
    
    auto whitePawns = board.getPieceBoard(Piece(WHITE, PAWN));
    auto blackPawns = board.getPieceBoard(Piece(BLACK, PAWN));
    auto whiteRooks = board.getPieceBoard(Piece(WHITE, ROOK));
    auto blackRooks = board.getPieceBoard(Piece(BLACK, ROOK));
    auto whiteKing = board.getPieceBoard(Piece(WHITE, KING));
    auto blackKing = board.getPieceBoard(Piece(BLACK, KING));
    
    const int centralBonus = 10;
    const int semiCentralBonus = 5;
    
    for (int file = 0; file < 8; file++) {
        auto fileMask = board.fileMasks[file];
        auto whitePawnsOnFile = fileMask & whitePawns;
        auto blackPawnsOnFile = fileMask & blackPawns;
        
        int whitePawnCount = __builtin_popcountll(whitePawnsOnFile);
        int blackPawnCount = __builtin_popcountll(blackPawnsOnFile);
        
        if (whitePawnCount > 1) {
            score += whitePawnCount * doublePawnPenalty[phaseIdx];
        }
        if (blackPawnCount > 1) {
            score -= blackPawnCount * doublePawnPenalty[phaseIdx];
        }
        
        bool isOpenFile = (whitePawnsOnFile == 0) && (blackPawnsOnFile == 0);
        bool isWhiteSemiOpen = (whitePawnsOnFile == 0) && (blackPawnsOnFile != 0);
        bool isBlackSemiOpen = (blackPawnsOnFile == 0) && (whitePawnsOnFile != 0);
        
        if (whiteRooks & fileMask) {
            if (isOpenFile) score += openFileBonus[phaseIdx];
            else if (isWhiteSemiOpen) score += semiOpenFileBonus[phaseIdx];
        }
        if (blackRooks & fileMask) {
            if (isOpenFile) score -= openFileBonus[phaseIdx];
            else if (isBlackSemiOpen) score -= semiOpenFileBonus[phaseIdx];
        }
        
        if (whiteKing & fileMask) {
            if (isOpenFile) score -= openFileBonus[phaseIdx] + 100;
            else if (isWhiteSemiOpen) score -= semiOpenFileBonus[phaseIdx] + 100;
        }
        if (blackKing & fileMask) {
            if (isOpenFile) score += openFileBonus[phaseIdx] + 100;
            else if (isBlackSemiOpen) score += semiOpenFileBonus[phaseIdx] + 100;
        }
    }
    
    BitBoard whitePawnBoard(whitePawns);
    BitBoard blackPawnBoard(blackPawns);
    
    for (int sq : centerSquares) {
        if (whitePawnBoard.getBoard() & (1ULL << sq)) score += centralBonus;
        if (blackPawnBoard.getBoard() & (1ULL << sq)) score -= centralBonus;
    }
    for (int sq : semiCenterSquares) {
        if (whitePawnBoard.getBoard() & (1ULL << sq)) score += semiCentralBonus;
        if (blackPawnBoard.getBoard() & (1ULL << sq)) score -= semiCentralBonus;
    }
    
    whitePawnBoard.setBoard(whitePawns);
    while (whitePawnBoard.getBoard()) {
        int sq = whitePawnBoard.LS1B();
        whitePawnBoard.flipBit(sq);
        
        if (!(board.isolatedMasks[sq] & whitePawns)) {
            score += isolatedPawnPenalty[phaseIdx];
        }
        
        if (!(board.whitePassedMasks[sq] & blackPawns)) {
            int rank = sq / 8;
            score += isMiddleGame ? middleGamePassedPawnBonus[rank] : endGamePassedPawnBonus[rank];
        }
    }
    
    blackPawnBoard.setBoard(blackPawns);
    while (blackPawnBoard.getBoard()) {
        int sq = blackPawnBoard.LS1B();
        blackPawnBoard.flipBit(sq);
        
        if (!(board.isolatedMasks[sq] & blackPawns)) {
            score -= isolatedPawnPenalty[phaseIdx];
        }
        
        if (!(board.blackPassedMasks[sq] & whitePawns)) {
            int rank = sq / 8;
            score -= isMiddleGame ? middleGamePassedPawnBonus[7 - rank] : endGamePassedPawnBonus[7 - rank];
        }
    }
    
    return score;
}

// ============================================================================
// MOBILITY AND CENTER CONTROL
// ============================================================================

int Evaluation::mobilityAndCenterControl(const ChessBoard& board, bool isMiddleGame) {
    int score = 0;
    auto allPieces = board.getAllPieces();
    
    int bishopMobilityBonus = isMiddleGame ? 5 : 3;
    int queenMobilityBonus = isMiddleGame ? 3 : 1;
    const int centralBonus = 10;
    const int semiCentralBonus = 5;
    
    // white bishops
    BitBoard whiteBishops(board.getPieceBoard(Piece(WHITE, BISHOP)));
    while (whiteBishops.getBoard()) {
        int sq = whiteBishops.LS1B();
        whiteBishops.flipBit(sq);
        
        auto attacks = board.bishopRestrictedAttackMasks(sq, allPieces);
        score += __builtin_popcountll(attacks) * bishopMobilityBonus;
        
        for (int centerSq : centerSquares) {
            if (attacks & (1ULL << centerSq)) score += centralBonus;
        }
        for (int semiSq : semiCenterSquares) {
            if (attacks & (1ULL << semiSq)) score += semiCentralBonus;
        }
    }
    
    // black bishops
    BitBoard blackBishops(board.getPieceBoard(Piece(BLACK, BISHOP)));
    while (blackBishops.getBoard()) {
        int sq = blackBishops.LS1B();
        blackBishops.flipBit(sq);
        
        auto attacks = board.bishopRestrictedAttackMasks(sq, allPieces);
        score -= __builtin_popcountll(attacks) * bishopMobilityBonus;
        
        for (int centerSq : centerSquares) {
            if (attacks & (1ULL << centerSq)) score -= centralBonus;
        }
        for (int semiSq : semiCenterSquares) {
            if (attacks & (1ULL << semiSq)) score -= semiCentralBonus;
        }
    }
    
    // white knights
    BitBoard whiteKnights(board.getPieceBoard(Piece(WHITE, KNIGHT)));
    while (whiteKnights.getBoard()) {
        int sq = whiteKnights.LS1B();
        whiteKnights.flipBit(sq);
        
        auto attacks = board.generateKnightAttacks(sq);
        
        for (int centerSq : centerSquares) {
            if (attacks & (1ULL << centerSq)) score += centralBonus;
        }
        for (int semiSq : semiCenterSquares) {
            if (attacks & (1ULL << semiSq)) score += semiCentralBonus;
        }
    }
    
    // BUG FIX: was using WHITE instead of BLACK
    BitBoard blackKnights(board.getPieceBoard(Piece(BLACK, KNIGHT)));
    while (blackKnights.getBoard()) {
        int sq = blackKnights.LS1B();
        blackKnights.flipBit(sq);
        
        auto attacks = board.generateKnightAttacks(sq);
        
        for (int centerSq : centerSquares) {
            if (attacks & (1ULL << centerSq)) score -= centralBonus;
        }
        for (int semiSq : semiCenterSquares) {
            if (attacks & (1ULL << semiSq)) score -= semiCentralBonus;
        }
    }
    
    // white queens
    BitBoard whiteQueen(board.getPieceBoard(Piece(WHITE, QUEEN)));
    while (whiteQueen.getBoard()) {
        int sq = whiteQueen.LS1B();
        whiteQueen.flipBit(sq);
        
        auto attacks = board.queenRestrictedAttackMasks(sq, allPieces);
        score += __builtin_popcountll(attacks) * queenMobilityBonus;
    }
    
    // black queens
    BitBoard blackQueen(board.getPieceBoard(Piece(BLACK, QUEEN)));
    while (blackQueen.getBoard()) {
        int sq = blackQueen.LS1B();
        blackQueen.flipBit(sq);
        
        auto attacks = board.queenRestrictedAttackMasks(sq, allPieces);
        score -= __builtin_popcountll(attacks) * queenMobilityBonus;
    }
    
    return score;
}

int Evaluation::kingSafety(const ChessBoard& board, bool isMiddleGame) {
    // placeholder - can be expanded
    return 0;
}

int Evaluation::evaluateOpenFiles(const ChessBoard& board, bool isMiddleGame) {
    // placeholder - already included in structuralEvaluation
    return 0;
}

// ============================================================================
// MVV-LVA
// ============================================================================

void Evaluation::initializeMVVLVA() {
    for (int victim = PAWN; victim <= KING; victim++) {
        for (int attacker = PAWN; attacker <= KING; attacker++) {
            middleGameMVVLVA[victim][attacker] = middleGamePieceValues[victim] + 5000 - middleGamePieceValues[attacker];
            endGameMVVLVA[victim][attacker] = endGamePieceValues[victim] + 5000 - endGamePieceValues[attacker];
        }
    }
}

// ============================================================================
// QUIESCENCE SEARCH
// ============================================================================

int Evaluation::quiescenceSearch(ChessBoard& board, int alpha, int beta, bool isWhite) {
    int standPat = evaluate(board);
    
    if (isWhite) {
        if (standPat >= beta) return beta;
        if (alpha < standPat) alpha = standPat;
    } else {
        if (standPat <= alpha) return alpha;
        if (beta > standPat) beta = standPat;
    }
    
    vector<Move> captures;
    board.generateCaptureMoves(isWhite ? WHITE : BLACK, captures);
    
    for (auto& move : captures) {
        board.saveBoardState();
        if (!board.makeAMove(move)) continue;
        
        int score = quiescenceSearch(board, alpha, beta, !isWhite);
        board.restoreBoardState();
        
        if (isWhite) {
            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        } else {
            if (score <= alpha) return alpha;
            if (score < beta) beta = score;
        }
    }
    
    return isWhite ? alpha : beta;
}

// ============================================================================
// MAIN SEARCH
// ============================================================================

SearchResult Evaluation::minMax(ChessBoard& board, int depth, int alpha, int beta, bool isWhite, vector<Move>& pv) {
    unsigned long long hash = board.currentHash;
    TranspositionTableEntry entry;
    
    if (depth == 0) {
        int eval = quiescenceSearch(board, alpha, beta, isWhite);
        return {eval, Move(), pv};
    }
    
    if (entryRetrieval(hash, entry, depth, alpha, beta)) {
        if (entry.evaluatedDepth >= depth) {
            if (entry.moveFlag == EXACT) {
                return {entry.positionScore, entry.bestMove, entry.pv};
            } else if (entry.moveFlag == LOWER) {
                alpha = max(alpha, entry.positionScore);
            } else if (entry.moveFlag == UPPER) {
                beta = min(beta, entry.positionScore);
            }
            
            if (alpha >= beta) {
                return {entry.positionScore, entry.bestMove, entry.pv};
            }
        }
    }
    
    bool foundLegalMove = false;
    SearchResult result;
    BoundFlag flag = isWhite ? LOWER : UPPER;
    
    vector<Move> moveList;
    vector<Move> currentPV;
    board.generateMoves(isWhite ? WHITE : BLACK, moveList);
    
    sortMove(moveList, depth, pv);
    
    int bestScore = isWhite ? INT_MIN : INT_MAX;
    Move bestMove;
    bool pvNodeFound = false;
    int moveIndex = 0;
    
    for (auto& move : moveList) {
        moveIndex++;
        
        board.saveBoardState();
        if (!board.makeAMove(move)) continue;
        
        foundLegalMove = true;
        
        if (pvNodeFound) {
            if (isWhite) {
                result = minMax(board, depth - 1, alpha, alpha + 1, !isWhite, currentPV);
            } else {
                result = minMax(board, depth - 1, beta - 1, beta, !isWhite, currentPV);
            }
            
            if (result.evaluation > alpha && result.evaluation < beta) {
                result = minMax(board, depth - 1, alpha, beta, !isWhite, currentPV);
            }
        } else {
            int reduction = (moveIndex > 0 && depth > 2) ? 2 : 0;
            result = minMax(board, depth - 1 - reduction, alpha, beta, !isWhite, currentPV);
        }
        
        board.restoreBoardState();
        
        if (isWhite) {
            if (result.evaluation > bestScore) {
                bestScore = result.evaluation;
                bestMove = move;
                
                pv.clear();
                pv.push_back(bestMove);
                pv.insert(pv.end(), result.pv.begin(), result.pv.end());
                
                pvNodeFound = true;
                flag = EXACT;
            }
            
            alpha = max(alpha, bestScore);
            
            if (beta <= alpha) {
                updateHistoryMoves(depth, move);
                updateKillerMoves(depth, move);
                flag = LOWER;
                break;
            }
        } else {
            if (result.evaluation < bestScore) {
                bestScore = result.evaluation;
                bestMove = move;
                
                pv.clear();
                pv.push_back(bestMove);
                pv.insert(pv.end(), result.pv.begin(), result.pv.end());
                
                pvNodeFound = true;
                flag = EXACT;
            }
            
            beta = min(beta, bestScore);
            
            if (beta <= alpha) {
                updateHistoryMoves(depth, move);
                updateKillerMoves(depth, move);
                flag = UPPER;
                break;
            }
        }
    }
    
    if (!foundLegalMove) {
        int kingSquare = BitBoard(board.getPieceBoard(Piece(isWhite ? WHITE : BLACK, KING))).LS1B();
        bool inCheck = board.isSquareAttacked(kingSquare, isWhite ? WHITE : BLACK);
        
        if (inCheck) {
            int mateScore = isWhite ? -MATE_VALUE + depth : MATE_VALUE - depth;
            entry = {hash, depth, mateScore, pv, Move(), EXACT};
            insertToTable(hash, entry);
            return {mateScore, Move(), pv};
        } else {
            entry = {hash, depth, 0, pv, Move(), EXACT};
            insertToTable(hash, entry);
            return {0, Move(), pv};
        }
    }
    
    if (isWhite) {
        flag = (bestScore >= beta) ? LOWER : ((bestScore <= alpha) ? UPPER : EXACT);
    } else {
        flag = (bestScore <= alpha) ? UPPER : ((bestScore >= beta) ? LOWER : EXACT);
    }
    
    entry = {hash, depth, bestScore, pv, bestMove, flag};
    insertToTable(hash, entry);
    
    return {bestScore, bestMove, pv};
}

// ============================================================================
// ITERATIVE DEEPENING
// ============================================================================

SearchResult Evaluation::iterativeDeepeningSearch(ChessBoard& board, int maxDepth, bool isWhite) {
    SearchResult finalResult;
    vector<Move> pv;
    
    auto start = high_resolution_clock::now();
    
    int aspirationWindow = 50;
    int previousEval = 0;
    
    for (int depth = 1; depth <= maxDepth; depth++) {
        cout << "Starting search at depth: " << depth << endl;
        
        int alpha = previousEval - aspirationWindow;
        int beta = previousEval + aspirationWindow;
        
        while (true) {
            finalResult = minMax(board, depth, alpha, beta, isWhite, pv);
            
            if (finalResult.evaluation <= alpha) {
                alpha = INT_MIN;
            } else if (finalResult.evaluation >= beta) {
                beta = INT_MAX;
            } else {
                break;
            }
        }
        
        previousEval = finalResult.evaluation;
        
        cout << "Depth: " << depth << " Best Move: ";
        board.printMove(finalResult.bestMove.startingSquare, finalResult.bestMove.endSquare);
        cout << " Evaluation: " << finalResult.evaluation << endl;
    }
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    
    cout << "Principal Variation: ";
    for (const Move& move : pv) {
        board.printMove(move.startingSquare, move.endSquare);
        cout << " ";
    }
    cout << endl;
    
    cout << "Time taken: " << duration.count() << " ms" << endl;
    
    return finalResult;
}

void Evaluation::printPVTable(int depth) {
    // placeholder
}

// ============================================================================
// MOVE ORDERING
// ============================================================================

int Evaluation::scoreMove(Move& move, int depth, const vector<Move>& pv) {
    if (pv.size() >= (size_t)depth && move == pv[depth - 1]) {
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

void Evaluation::sortMove(vector<Move>& moveList, int depth, const vector<Move>& pv) {
    std::sort(moveList.begin(), moveList.end(), [this, depth, &pv](Move& a, Move& b) {
        return scoreMove(a, depth, pv) > scoreMove(b, depth, pv);
    });
}

void Evaluation::updateKillerMoves(int depth, const Move& move) {
    if (move.capturedPiece.type != EMPTY) return;
    if (killerMoves[depth][0] == move || killerMoves[depth][1] == move) return;
    
    killerMoves[depth][1] = killerMoves[depth][0];
    killerMoves[depth][0] = move;
}

void Evaluation::updateHistoryMoves(int depth, const Move& move) {
    if (move.capturedPiece.type != EMPTY) return;
    historyMoves[move.movingPieceColor][move.movingPieceType][move.endSquare] += depth * depth;
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void Evaluation::initializeKillerMoves() {
    Move emptyMove(-1, -1, EMPTY, WHITE, Piece(), EMPTY, false, false, false, false);
    
    for (auto& movesAtDepth : killerMoves) {
        movesAtDepth[0] = emptyMove;
        movesAtDepth[1] = emptyMove;
    }
}

void Evaluation::initializeHistoryMoves() {
    for (auto& colorTable : historyMoves) {
        for (auto& pieceTable : colorTable) {
            std::fill(pieceTable.begin(), pieceTable.end(), 0);
        }
    }
}

// ============================================================================
// TRANSPOSITION TABLE
// ============================================================================

void Evaluation::clearTranspositionTable() {
    transpositionTable.clear();
}

void Evaluation::insertToTable(unsigned long long hash, TranspositionTableEntry& entry) {
    if (entry.positionScore < -MATE_SCORE) {
        entry.positionScore -= entry.evaluatedDepth;
    }
    if (entry.positionScore > MATE_SCORE) {
        entry.positionScore += entry.evaluatedDepth;
    }
    
    auto& entries = transpositionTable[hash];
    
    auto it = std::find_if(entries.begin(), entries.end(), [&](const TranspositionTableEntry& e) {
        return e.hash == hash;
    });
    
    if (it != entries.end()) {
        if (entry.evaluatedDepth > it->evaluatedDepth ||
            (entry.evaluatedDepth == it->evaluatedDepth && entry.moveFlag == EXACT)) {
            *it = entry;
        }
    } else {
        entries.push_back(entry);
    }
}

bool Evaluation::entryRetrieval(unsigned long long hash, TranspositionTableEntry& entry, 
                                 int depth, int alpha, int beta) const {
    auto it = transpositionTable.find(hash);
    if (it == transpositionTable.end()) return false;
    
    for (const auto& e : it->second) {
        if (e.evaluatedDepth >= depth) {
            entry = e;
            
            if (entry.positionScore > MATE_SCORE) {
                entry.positionScore -= entry.evaluatedDepth;
            }
            if (entry.positionScore < -MATE_SCORE) {
                entry.positionScore += entry.evaluatedDepth;
            }
            
            return true;
        }
    }
    
    return false;
}