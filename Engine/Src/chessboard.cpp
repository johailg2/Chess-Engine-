#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <iterator>
#include <random>
#include <sstream>
#include "../headers/chessboard.h"
#include "../headers/attackTables.h"
#include "../headers/bitboards.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

// ============================================================================
// INITIALIZATION
// ============================================================================

ChessBoard::ChessBoard() {
    // starting position - white on bottom (ranks 1-2), black on top (ranks 7-8)
    // bit 0 is h1, bit 63 is a8
    auto whitePawns   = 0x000000000000FF00ULL;
    auto whiteRooks   = 0x0000000000000081ULL;
    auto whiteKnights = 0x0000000000000042ULL;
    auto whiteBishops = 0x0000000000000024ULL;
    auto whiteQueen   = 0x0000000000000010ULL;
    auto whiteKing    = 0x0000000000000008ULL;

    auto blackPawns   = 0x00FF000000000000ULL;
    auto blackRooks   = 0x8100000000000000ULL;
    auto blackKnights = 0x4200000000000000ULL;
    auto blackBishops = 0x2400000000000000ULL;
    auto blackQueen   = 0x1000000000000000ULL;
    auto blackKing    = 0x0800000000000000ULL;

    allWhitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueen | whiteKing;
    allBlackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueen | blackKing;
    allPieces = allWhitePieces | allBlackPieces;

    colorToMove = WHITE;
    
    canWhiteCastleKingSide = true;
    canBlackCastleKingSide = true;
    canWhiteCastleQueenSide = true;
    canBlackCastleQueenSide = true;

    isThereCheck = false;
    enPassantFile = -1;
    
    // index 0 = pawns, 1 = rooks, 2 = knights, 3 = bishops, 4 = queen, 5 = king
    whiteBoards = {whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing};
    blackBoards = {blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing};
}

void ChessBoard::initialize() {
    // order matters - masks first, then magic tables, then attack boards
    initializeAllSlidingMasks();
    initializeAttackTables(1);  // bishops
    initializeAttackTables(0);  // rooks
    
    initializeAllSquaresAttackedByBlack();
    initializeAllSquaresAttackedByWhite();
    
    initializeFileMasks();
    initializeRankMasks();
    initializeIsolatedMasks();
    initializeWhitePassedPawnMasks();
    initializeBlackPassedPawnMasks();
    
    initializeHashKeys();
    currentHash = generateInitialHash();
}

// ============================================================================
// PIECE ACCESS
// ============================================================================

unsigned long long ChessBoard::getPieceBoard(Piece piece) const {
    auto& boards = (piece.color == WHITE) ? whiteBoards : blackBoards;
    
    switch (piece.type) {
        case PAWN:   return boards[0];
        case ROOK:   return boards[1];
        case KNIGHT: return boards[2];
        case BISHOP: return boards[3];
        case QUEEN:  return boards[4];
        case KING:   return boards[5];
        default:     return 0ULL;
    }
}

Piece ChessBoard::getPiece(int square) const {
    unsigned long long mask = 1ULL << square;
    
    for (int i = 0; i < 6; ++i) {
        if (whiteBoards[i] & mask) {
            return Piece(WHITE, static_cast<PieceType>(i));
        }
        if (blackBoards[i] & mask) {
            return Piece(BLACK, static_cast<PieceType>(i));
        }
    }
    return Piece();
}

void ChessBoard::setPieceBoard(Piece piece, unsigned long long newBoard) {
    auto& boards = (piece.color == WHITE) ? whiteBoards : blackBoards;
    
    switch (piece.type) {
        case PAWN:   boards[0] = newBoard; break;
        case ROOK:   boards[1] = newBoard; break;
        case KNIGHT: boards[2] = newBoard; break;
        case BISHOP: boards[3] = newBoard; break;
        case QUEEN:  boards[4] = newBoard; break;
        case KING:   boards[5] = newBoard; break;
        default: break;
    }
}

void ChessBoard::removePiece(int square) {
    unsigned long long mask = ~(1ULL << square);
    for (auto& board : whiteBoards) board &= mask;
    for (auto& board : blackBoards) board &= mask;
}

void ChessBoard::placePiece(int square, Piece piece) {
    unsigned long long mask = 1ULL << square;
    auto& boards = (piece.color == WHITE) ? whiteBoards : blackBoards;
    
    switch (piece.type) {
        case PAWN:   boards[0] |= mask; break;
        case ROOK:   boards[1] |= mask; break;
        case KNIGHT: boards[2] |= mask; break;
        case BISHOP: boards[3] |= mask; break;
        case QUEEN:  boards[4] |= mask; break;
        case KING:   boards[5] |= mask; break;
        default: break;
    }
}

void ChessBoard::clearBitForAllPieces(int square, Color color) {
    auto& boards = (color == WHITE) ? whiteBoards : blackBoards;
    for (auto& board : boards) {
        board &= ~(1ULL << square);
    }
}

// ============================================================================
// ATTACK GENERATION
// ============================================================================

unsigned long long ChessBoard::pawnAttacks(int square, Color color) {
    unsigned long long pawn = 1ULL << square;
    unsigned long long attacks = 0ULL;
    
    if (color == WHITE) {
        attacks |= (pawn << 7) & NOT_A_FILE;
        attacks |= (pawn << 9) & NOT_H_FILE;
    } else {
        attacks |= (pawn >> 7) & NOT_H_FILE;
        attacks |= (pawn >> 9) & NOT_A_FILE;
    }
    return attacks;
}

unsigned long long ChessBoard::generateKnightAttacks(int square) const {
    unsigned long long knight = 1ULL << square;
    unsigned long long attacks = 0ULL;
    
    attacks |= (knight << 17) & NOT_H_FILE;
    attacks |= (knight << 15) & NOT_A_FILE;
    attacks |= (knight << 10) & NOT_GH_FILE;
    attacks |= (knight << 6)  & NOT_AB_FILE;
    attacks |= (knight >> 17) & NOT_A_FILE;
    attacks |= (knight >> 15) & NOT_H_FILE;
    attacks |= (knight >> 10) & NOT_AB_FILE;
    attacks |= (knight >> 6)  & NOT_GH_FILE;
    
    return attacks;
}

unsigned long long ChessBoard::generateKingAttacks(int square) {
    unsigned long long king = 1ULL << square;
    unsigned long long attacks = 0ULL;
    
    attacks |= (king >> 8);
    attacks |= (king << 8);
    attacks |= (king >> 1) & NOT_A_FILE;
    attacks |= (king << 1) & NOT_H_FILE;
    attacks |= (king >> 9) & NOT_A_FILE;
    attacks |= (king >> 7) & NOT_H_FILE;
    attacks |= (king << 7) & NOT_A_FILE;
    attacks |= (king << 9) & NOT_H_FILE;
    
    return attacks;
}

// ============================================================================
// SLIDING PIECE ATTACKS - for magic bitboard generation
// ============================================================================

unsigned long long ChessBoard::bishopAttackMasks(int square) {
    unsigned long long attacks = 0ULL;
    int row = square / 8;
    int col = square % 8;
    
    for (int r = row + 1, c = col + 1; r < 7 && c < 7; r++, c++)
        attacks |= 1ULL << (r * 8 + c);
    for (int r = row + 1, c = col - 1; r < 7 && c > 0; r++, c--)
        attacks |= 1ULL << (r * 8 + c);
    for (int r = row - 1, c = col + 1; r > 0 && c < 7; r--, c++)
        attacks |= 1ULL << (r * 8 + c);
    for (int r = row - 1, c = col - 1; r > 0 && c > 0; r--, c--)
        attacks |= 1ULL << (r * 8 + c);
    
    return attacks;
}

unsigned long long ChessBoard::bishopRestrictedAttackMasks(int square, unsigned long long blockers) const {
    unsigned long long attacks = 0ULL;
    int row = square / 8;
    int col = square % 8;
    
    for (int r = row + 1, c = col + 1; r < 8 && c < 8; r++, c++) {
        attacks |= 1ULL << (r * 8 + c);
        if ((1ULL << (r * 8 + c)) & blockers) break;
    }
    for (int r = row + 1, c = col - 1; r < 8 && c >= 0; r++, c--) {
        attacks |= 1ULL << (r * 8 + c);
        if ((1ULL << (r * 8 + c)) & blockers) break;
    }
    for (int r = row - 1, c = col + 1; r >= 0 && c < 8; r--, c++) {
        attacks |= 1ULL << (r * 8 + c);
        if ((1ULL << (r * 8 + c)) & blockers) break;
    }
    for (int r = row - 1, c = col - 1; r >= 0 && c >= 0; r--, c--) {
        attacks |= 1ULL << (r * 8 + c);
        if ((1ULL << (r * 8 + c)) & blockers) break;
    }
    
    return attacks;
}

unsigned long long ChessBoard::rookAttackMasks(int square) {
    unsigned long long attacks = 0ULL;
    int row = square / 8;
    int col = square % 8;
    
    for (int r = row + 1; r < 7; r++) attacks |= 1ULL << (r * 8 + col);
    for (int r = row - 1; r > 0; r--) attacks |= 1ULL << (r * 8 + col);
    for (int c = col + 1; c < 7; c++) attacks |= 1ULL << (row * 8 + c);
    for (int c = col - 1; c > 0; c--) attacks |= 1ULL << (row * 8 + c);
    
    return attacks;
}

unsigned long long ChessBoard::rookRestrictedAttacksMasks(int square, unsigned long long blockers) const {
    unsigned long long attacks = 0ULL;
    int row = square / 8;
    int col = square % 8;
    
    for (int r = row + 1; r < 8; r++) {
        attacks |= 1ULL << (r * 8 + col);
        if ((1ULL << (r * 8 + col)) & blockers) break;
    }
    for (int r = row - 1; r >= 0; r--) {
        attacks |= 1ULL << (r * 8 + col);
        if ((1ULL << (r * 8 + col)) & blockers) break;
    }
    for (int c = col + 1; c < 8; c++) {
        attacks |= 1ULL << (row * 8 + c);
        if ((1ULL << (row * 8 + c)) & blockers) break;
    }
    for (int c = col - 1; c >= 0; c--) {
        attacks |= 1ULL << (row * 8 + c);
        if ((1ULL << (row * 8 + c)) & blockers) break;
    }
    
    return attacks;
}

unsigned long long ChessBoard::queenAttackMasks(int square) {
    return rookAttackMasks(square) | bishopAttackMasks(square);
}

unsigned long long ChessBoard::queenRestrictedAttackMasks(int square, unsigned long long blockers) const {
    return rookRestrictedAttacksMasks(square, blockers) | bishopRestrictedAttackMasks(square, blockers);
}

// ============================================================================
// MAGIC BITBOARDS
// ============================================================================

unsigned long long attackMaskToOccupancy(int index, int numBits, BitBoard attackMask) {
    unsigned long long occupancy = 0ULL;
    
    for (int bit = 0; bit < numBits; bit++) {
        int sq = attackMask.LS1B();
        attackMask.flipBit(sq);
        if (index & (1 << bit)) {
            occupancy |= (1ULL << sq);
        }
    }
    return occupancy;
}

unsigned long long randomUInt64() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<unsigned long long> dis;
    return dis(gen);
}

unsigned long long randomFewBits() {
    return randomUInt64() & randomUInt64() & randomUInt64();
}

unsigned long long ChessBoard::findMagicNumber(int square, int relevantBits, bool bishop) {
    unsigned long long mask = bishop ? bishopAttackMasks(square) : rookAttackMasks(square);
    unsigned long long attackSet[4096], usedAttacks[4096], occupancySet[4096];
    int occupancyCount = 1 << relevantBits;
    
    for (int i = 0; i < occupancyCount; i++) {
        occupancySet[i] = attackMaskToOccupancy(i, relevantBits, mask);
        attackSet[i] = bishop ? bishopRestrictedAttackMasks(square, occupancySet[i])
                              : rookRestrictedAttacksMasks(square, occupancySet[i]);
    }
    
    for (int attempt = 0; attempt < 100000000; attempt++) {
        unsigned long long magic = randomFewBits();
        
        if (__builtin_popcountll((mask * magic) & 0xFF00000000000000ULL) < 6)
            continue;
        
        std::fill(std::begin(usedAttacks), std::end(usedAttacks), 0ULL);
        
        bool collision = false;
        for (int i = 0; i < occupancyCount && !collision; i++) {
            int magicIndex = (int)((occupancySet[i] * magic) >> (64 - relevantBits));
            
            if (usedAttacks[magicIndex] == 0ULL) {
                usedAttacks[magicIndex] = attackSet[i];
            } else if (usedAttacks[magicIndex] != attackSet[i]) {
                collision = true;
            }
        }
        
        if (!collision) return magic;
    }
    
    cout << "***Failed to find magic number***" << endl;
    return 0ULL;
}

void ChessBoard::initializeAllSlidingMasks() {
    for (int sq = 0; sq < 64; sq++) {
        allBishopMasks[sq] = bishopAttackMasks(sq);
        allRookMasks[sq] = rookAttackMasks(sq);
    }
}

void ChessBoard::initializeAttackTables(int bishop) {
    for (int square = 0; square < 64; square++) {
        unsigned long long mask = bishop ? allBishopMasks[square] : allRookMasks[square];
        int relevantBits = __builtin_popcountll(mask);
        int occupancyCount = 1 << relevantBits;
        
        for (int index = 0; index < occupancyCount; index++) {
            auto occupancy = attackMaskToOccupancy(index, relevantBits, mask);
            
            if (bishop) {
                int magicIndex = (occupancy * bishopMagics[square]) >> (64 - relevantBits);
                magicBishopAttacks[square][magicIndex] = bishopRestrictedAttackMasks(square, occupancy);
            } else {
                int magicIndex = (occupancy * rookMagics[square]) >> (64 - relevantBits);
                magicRookAttacks[square][magicIndex] = rookRestrictedAttacksMasks(square, occupancy);
            }
        }
    }
}

unsigned long long ChessBoard::instantBishopAttacks(unsigned long long occupancy, int square) {
    occupancy &= allBishopMasks[square];
    int magicIndex = (occupancy * bishopMagics[square]) >> (64 - __builtin_popcountll(allBishopMasks[square]));
    return magicBishopAttacks[square][magicIndex];
}

unsigned long long ChessBoard::instantRookAttacks(unsigned long long occupancy, int square) {
    occupancy &= allRookMasks[square];
    int magicIndex = (occupancy * rookMagics[square]) >> (64 - __builtin_popcountll(allRookMasks[square]));
    return magicRookAttacks[square][magicIndex];
}

unsigned long long ChessBoard::instantQueenAttacks(unsigned long long occupancy, int square) {
    return instantBishopAttacks(occupancy, square) | instantRookAttacks(occupancy, square);
}

// ============================================================================
// ATTACK TRACKING
// ============================================================================

void ChessBoard::initializeAllSquaresAttackedByWhite() {
    allSquaresAttackedByWhite = 0ULL;
    
    BitBoard pieces(whiteBoards[0]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByWhite |= pawnAttacks(sq, WHITE);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(whiteBoards[2]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByWhite |= generateKnightAttacks(sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(whiteBoards[3]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByWhite |= instantBishopAttacks(getAllPieces(), sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(whiteBoards[1]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByWhite |= instantRookAttacks(getAllPieces(), sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(whiteBoards[4]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByWhite |= instantQueenAttacks(getAllPieces(), sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(whiteBoards[5]);
    if (pieces.getBoard()) {
        allSquaresAttackedByWhite |= generateKingAttacks(pieces.LS1B());
    }
}

void ChessBoard::initializeAllSquaresAttackedByBlack() {
    allSquaresAttackedByBlack = 0ULL;
    
    BitBoard pieces(blackBoards[0]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByBlack |= pawnAttacks(sq, BLACK);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(blackBoards[2]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByBlack |= generateKnightAttacks(sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(blackBoards[3]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByBlack |= instantBishopAttacks(getAllPieces(), sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(blackBoards[1]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByBlack |= instantRookAttacks(getAllPieces(), sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(blackBoards[4]);
    while (pieces.getBoard()) {
        int sq = pieces.LS1B();
        allSquaresAttackedByBlack |= instantQueenAttacks(getAllPieces(), sq);
        pieces.flipBit(sq);
    }
    
    pieces.setBoard(blackBoards[5]);
    if (pieces.getBoard()) {
        allSquaresAttackedByBlack |= generateKingAttacks(pieces.LS1B());
    }
}

bool ChessBoard::isSquareAttacked(int square, Color friendlyColor) {
    if (friendlyColor == WHITE) {
        return (allSquaresAttackedByBlack & (1ULL << square)) != 0;
    } else {
        return (allSquaresAttackedByWhite & (1ULL << square)) != 0;
    }
}

unsigned long long ChessBoard::getAllSquaresAttackedByWhite() {
    return allSquaresAttackedByWhite;
}

unsigned long long ChessBoard::getAllSquaresAttackedByBlack() {
    return allSquaresAttackedByBlack;
}

// ============================================================================
// MOVE GENERATION
// ============================================================================

void ChessBoard::generateMoves(Color color, vector<Move>& moveList) {
    moveList.clear();
    populatePawnCaptures(color, lastMove, moveList);
    populateQuietPawnMoves(color, moveList);
    populateKnightMoves(color, moveList, false);
    populateBishopMoves(color, moveList, false);
    populateQueenMoves(color, moveList, false);
    populateRookMoves(color, moveList, false);
    populateKingMoves(color, moveList, false);
}

void ChessBoard::generateCaptureMoves(Color color, vector<Move>& moveList) {
    moveList.clear();
    populatePawnCaptures(color, lastMove, moveList);
    populateKnightMoves(color, moveList, true);
    populateBishopMoves(color, moveList, true);
    populateQueenMoves(color, moveList, true);
    populateRookMoves(color, moveList, true);
    populateKingMoves(color, moveList, true);
}

void ChessBoard::populatePawnCaptures(Color color, Move lastMove, vector<Move>& moveList) {
    BitBoard pawnBoard = (color == WHITE) ? BitBoard(whiteBoards[0]) : BitBoard(blackBoards[0]);
    int promotionRank = (color == WHITE) ? 7 : 0;
    
    // en passant
    if (lastMove.doublePawn) {
        int direction = (color == WHITE) ? 1 : -1;
        int epRank = (color == WHITE) ? 4 : 3;
        
        if (lastMove.endSquare / 8 == epRank) {
            for (int offset : {-1, 1}) {
                int neighborSq = lastMove.endSquare + offset;
                if (neighborSq < 0 || neighborSq >= 64) continue;
                if (abs((lastMove.endSquare % 8) - (neighborSq % 8)) != 1) continue;
                
                if (pawnBoard.getBoard() & (1ULL << neighborSq)) {
                    int epTargetSq = lastMove.endSquare + (direction * 8);
                    moveList.emplace_back(Move(neighborSq, epTargetSq, PAWN, color, 
                                               getPiece(lastMove.endSquare), EMPTY, 0, 0, 1, 0));
                }
            }
        }
    }
    
    // regular captures
    while (pawnBoard.getBoard()) {
        int fromSq = pawnBoard.LS1B();
        pawnBoard.flipBit(fromSq);
        
        BitBoard attackBoard(pawnAttacks(fromSq, color) & getOpposingBoard(color));
        
        while (attackBoard.getBoard()) {
            int toSq = attackBoard.LS1B();
            attackBoard.flipBit(toSq);
            
            if (abs((fromSq % 8) - (toSq % 8)) != 1) continue;
            
            if (toSq / 8 == promotionRank) {
                for (PieceType promo : {ROOK, KNIGHT, BISHOP, QUEEN}) {
                    moveList.emplace_back(Move(fromSq, toSq, PAWN, color, 
                                               getPiece(toSq), promo, 1, 0, 0, 0));
                }
            } else {
                moveList.emplace_back(Move(fromSq, toSq, PAWN, color, 
                                           getPiece(toSq), EMPTY, 0, 0, 0, 0));
            }
        }
    }
}

void ChessBoard::populateQuietPawnMoves(Color color, vector<Move>& moveList) {
    BitBoard pawnBoard = (color == WHITE) ? BitBoard(whiteBoards[0]) : BitBoard(blackBoards[0]);
    int direction = (color == WHITE) ? 8 : -8;
    int startRank = (color == WHITE) ? 1 : 6;
    int promotionRank = (color == WHITE) ? 7 : 0;
    
    while (pawnBoard.getBoard()) {
        int fromSq = pawnBoard.LS1B();
        pawnBoard.flipBit(fromSq);
        
        int singlePushSq = fromSq + direction;
        if (singlePushSq < 0 || singlePushSq >= 64) continue;
        
        if (!(allPieces & (1ULL << singlePushSq))) {
            if (singlePushSq / 8 == promotionRank) {
                for (PieceType promo : {ROOK, KNIGHT, BISHOP, QUEEN}) {
                    moveList.emplace_back(Move(fromSq, singlePushSq, PAWN, color, 
                                               Piece(), promo, 1, 0, 0, 0));
                }
            } else {
                moveList.emplace_back(Move(fromSq, singlePushSq, PAWN, color, 
                                           Piece(), EMPTY, 0, 0, 0, 0));
            }
            
            // double push from starting rank
            if (fromSq / 8 == startRank) {
                int doublePushSq = fromSq + (direction * 2);
                
                if (!(allPieces & (1ULL << doublePushSq))) {
                    Move doublePush(fromSq, doublePushSq, PAWN, color, Piece(), EMPTY, 0, 1, 0, 0);
                    moveList.emplace_back(doublePush);
                    
                    auto enemyPawns = (color == WHITE) ? blackBoards[0] : whiteBoards[0];
                    int leftSq = doublePushSq - 1;
                    int rightSq = doublePushSq + 1;
                    
                    bool leftAdjacent = (leftSq >= 0) && (doublePushSq % 8 != 0) && (enemyPawns & (1ULL << leftSq));
                    bool rightAdjacent = (rightSq < 64) && (doublePushSq % 8 != 7) && (enemyPawns & (1ULL << rightSq));
                    
                    if (leftAdjacent || rightAdjacent) {
                        enPassantEnable.emplace_back(doublePush);
                    }
                }
            }
        }
    }
}

void ChessBoard::populateKnightMoves(Color color, vector<Move>& moveList, bool captureOnly) {
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard knightBoard(getPieceBoard(Piece(color, KNIGHT)));
    
    while (knightBoard.getBoard()) {
        int fromSq = knightBoard.LS1B();
        knightBoard.flipBit(fromSq);
        
        unsigned long long attacks = generateKnightAttacks(fromSq);
        
        BitBoard captureBoard(attacks & opposingBoard);
        while (captureBoard.getBoard()) {
            int toSq = captureBoard.LS1B();
            captureBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, KNIGHT, color, getPiece(toSq), EMPTY, 0, 0, 0, 0));
        }
        
        if (captureOnly) continue;
        
        BitBoard quietBoard((attacks & ~friendlyBoard) & ~opposingBoard);
        while (quietBoard.getBoard()) {
            int toSq = quietBoard.LS1B();
            quietBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, KNIGHT, color, Piece(), EMPTY, 0, 0, 0, 0));
        }
    }
}

void ChessBoard::populateBishopMoves(Color color, vector<Move>& moveList, bool captureOnly) {
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard bishopBoard(getPieceBoard(Piece(color, BISHOP)));
    
    while (bishopBoard.getBoard()) {
        int fromSq = bishopBoard.LS1B();
        bishopBoard.flipBit(fromSq);
        
        unsigned long long attacks = instantBishopAttacks(getAllPieces(), fromSq);
        
        BitBoard captureBoard(attacks & opposingBoard);
        while (captureBoard.getBoard()) {
            int toSq = captureBoard.LS1B();
            captureBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, BISHOP, color, getPiece(toSq), EMPTY, 0, 0, 0, 0));
        }
        
        if (captureOnly) continue;
        
        BitBoard quietBoard((attacks & ~friendlyBoard) & ~opposingBoard);
        while (quietBoard.getBoard()) {
            int toSq = quietBoard.LS1B();
            quietBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, BISHOP, color, Piece(), EMPTY, 0, 0, 0, 0));
        }
    }
}

void ChessBoard::populateRookMoves(Color color, vector<Move>& moveList, bool captureOnly) {
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard rookBoard(getPieceBoard(Piece(color, ROOK)));
    
    while (rookBoard.getBoard()) {
        int fromSq = rookBoard.LS1B();
        rookBoard.flipBit(fromSq);
        
        unsigned long long attacks = instantRookAttacks(getAllPieces(), fromSq);
        
        BitBoard captureBoard(attacks & opposingBoard);
        while (captureBoard.getBoard()) {
            int toSq = captureBoard.LS1B();
            captureBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, ROOK, color, getPiece(toSq), EMPTY, 0, 0, 0, 0));
        }
        
        if (captureOnly) continue;
        
        BitBoard quietBoard((attacks & ~friendlyBoard) & ~opposingBoard);
        while (quietBoard.getBoard()) {
            int toSq = quietBoard.LS1B();
            quietBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, ROOK, color, Piece(), EMPTY, 0, 0, 0, 0));
        }
    }
}

// BUG FIX: was using 'return' instead of 'continue' when captureOnly is true
// this caused the function to exit after processing just one queen
void ChessBoard::populateQueenMoves(Color color, vector<Move>& moveList, bool captureOnly) {
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard queenBoard(getPieceBoard(Piece(color, QUEEN)));
    
    while (queenBoard.getBoard()) {
        int fromSq = queenBoard.LS1B();
        queenBoard.flipBit(fromSq);
        
        unsigned long long attacks = instantQueenAttacks(getAllPieces(), fromSq);
        
        BitBoard captureBoard(attacks & opposingBoard);
        while (captureBoard.getBoard()) {
            int toSq = captureBoard.LS1B();
            captureBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, QUEEN, color, getPiece(toSq), EMPTY, 0, 0, 0, 0));
        }
        
        // FIX: changed from 'return' to 'continue' to handle multiple queens
        if (captureOnly) continue;
        
        BitBoard quietBoard((attacks & ~friendlyBoard) & ~opposingBoard);
        while (quietBoard.getBoard()) {
            int toSq = quietBoard.LS1B();
            quietBoard.flipBit(toSq);
            moveList.emplace_back(Move(fromSq, toSq, QUEEN, color, Piece(), EMPTY, 0, 0, 0, 0));
        }
    }
}

void ChessBoard::populateKingMoves(Color color, vector<Move>& moveList, bool captureOnly) {
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard kingBoard((color == WHITE) ? whiteBoards[5] : blackBoards[5]);
    
    int kingSq = kingBoard.LS1B();
    unsigned long long attacks = generateKingAttacks(kingSq);
    
    BitBoard captureBoard(attacks & opposingBoard);
    while (captureBoard.getBoard()) {
        int toSq = captureBoard.LS1B();
        captureBoard.flipBit(toSq);
        
        if (isSquareAttacked(toSq, color)) continue;
        moveList.emplace_back(Move(kingSq, toSq, KING, color, getPiece(toSq), EMPTY, 0, 0, 0, 0));
    }
    
    if (captureOnly) return;
    
    // castling
    if (color == WHITE) {
        if (canWhiteCastleQueenSide) {
            bool pathClear = !(allPieces & ((1ULL << (kingSq + 1)) | (1ULL << (kingSq + 2))));
            bool pathSafe = !isSquareAttacked(kingSq, WHITE) && 
                            !isSquareAttacked(kingSq + 1, WHITE) && 
                            !isSquareAttacked(kingSq + 2, WHITE);
            if (pathClear && pathSafe) {
                moveList.emplace_back(Move(kingSq, kingSq + 2, KING, WHITE, Piece(), EMPTY, 0, 0, 0, 1));
            }
        }
        if (canWhiteCastleKingSide) {
            bool pathClear = !(allPieces & ((1ULL << (kingSq - 1)) | (1ULL << (kingSq - 2))));
            bool pathSafe = !isSquareAttacked(kingSq, WHITE) && 
                            !isSquareAttacked(kingSq - 1, WHITE) && 
                            !isSquareAttacked(kingSq - 2, WHITE);
            if (pathClear && pathSafe) {
                moveList.emplace_back(Move(kingSq, kingSq - 2, KING, WHITE, Piece(), EMPTY, 0, 0, 0, 1));
            }
        }
    } else {
        if (canBlackCastleKingSide) {
            bool pathClear = !(allPieces & ((1ULL << (kingSq - 1)) | (1ULL << (kingSq - 2))));
            bool pathSafe = !isSquareAttacked(kingSq, BLACK) && 
                            !isSquareAttacked(kingSq - 1, BLACK) && 
                            !isSquareAttacked(kingSq - 2, BLACK);
            if (pathClear && pathSafe) {
                moveList.emplace_back(Move(kingSq, kingSq - 2, KING, BLACK, Piece(), EMPTY, 0, 0, 0, 1));
            }
        }
        if (canBlackCastleQueenSide) {
            bool pathClear = !(allPieces & ((1ULL << (kingSq + 1)) | (1ULL << (kingSq + 2))));
            bool pathSafe = !isSquareAttacked(kingSq, BLACK) && 
                            !isSquareAttacked(kingSq + 1, BLACK) && 
                            !isSquareAttacked(kingSq + 2, BLACK);
            if (pathClear && pathSafe) {
                moveList.emplace_back(Move(kingSq, kingSq + 2, KING, BLACK, Piece(), EMPTY, 0, 0, 0, 1));
            }
        }
    }
    
    BitBoard quietBoard((attacks & ~friendlyBoard) & ~opposingBoard);
    while (quietBoard.getBoard()) {
        int toSq = quietBoard.LS1B();
        quietBoard.flipBit(toSq);
        
        if (isSquareAttacked(toSq, color)) continue;
        moveList.emplace_back(Move(kingSq, toSq, KING, color, Piece(), EMPTY, 0, 0, 0, 0));
    }
}

// ============================================================================
// MAKING AND UNMAKING MOVES
// ============================================================================

bool ChessBoard::makeAMove(Move& move) {
    currentHash = updateHash(currentHash, move);
    repetitionTable[currentHash]++;
    
    removePiece(move.endSquare);
    removePiece(move.startingSquare);
    
    if (move.isPromotion) {
        placePiece(move.endSquare, Piece(move.movingPieceColor, move.promotedPieceType));
    } else {
        placePiece(move.endSquare, Piece(move.movingPieceColor, move.movingPieceType));
    }
    
    if (move.movingPieceType == KING) {
        if (move.movingPieceColor == WHITE) {
            canWhiteCastleKingSide = false;
            canWhiteCastleQueenSide = false;
        } else {
            canBlackCastleKingSide = false;
            canBlackCastleQueenSide = false;
        }
    }
    
    enPassantFile = -1;
    for (auto& m : enPassantEnable) {
        if (m == move) {
            enPassantFile = move.endSquare % 8;
            break;
        }
    }
    
    // castling rights on rook move
    if (move.movingPieceType == ROOK) {
        if (move.startingSquare == H8) canBlackCastleKingSide = false;
        else if (move.startingSquare == A8) canBlackCastleQueenSide = false;
        else if (move.startingSquare == H1) canWhiteCastleKingSide = false;
        else if (move.startingSquare == A1) canWhiteCastleQueenSide = false;
    }
    
    // castling rights on rook capture
    if (move.capturedPiece.type == ROOK) {
        if (move.endSquare == H8) canBlackCastleKingSide = false;
        else if (move.endSquare == A8) canBlackCastleQueenSide = false;
        else if (move.endSquare == H1) canWhiteCastleKingSide = false;
        else if (move.endSquare == A1) canWhiteCastleQueenSide = false;
    }
    
    if (move.enpassant) {
        handleEnpassant(move.endSquare, move.movingPieceColor);
    }
    if (move.castling) {
        handleCastling(move.startingSquare, move.endSquare, move.movingPieceColor);
    }
    
    updateBoards();
    
    int ourKingSq = BitBoard((move.movingPieceColor == WHITE) ? whiteBoards[5] : blackBoards[5]).LS1B();
    if (isSquareAttacked(ourKingSq, move.movingPieceColor)) {
        undoMove();
        return false;
    }
    
    int theirKingSq = BitBoard((move.movingPieceColor == BLACK) ? whiteBoards[5] : blackBoards[5]).LS1B();
    isThereCheck = isSquareAttacked(theirKingSq, (move.movingPieceColor == WHITE) ? BLACK : WHITE);
    
    if (move.capturedPiece.type != EMPTY) {
        captures++;
    }
    
    colorToMove = (move.movingPieceColor == WHITE) ? BLACK : WHITE;
    lastMove = move;
    enPassantEnable.clear();
    
    return true;
}

void ChessBoard::undoMove() {
    restoreBoardState();
}

bool ChessBoard::makeNullMove() {
    currentHash ^= turnKey;
    enPassantFile = -1;
    return true;
}

void ChessBoard::handleEnpassant(int endSquare, Color color) {
    int capturedPawnSq = (color == WHITE) ? endSquare - 8 : endSquare + 8;
    removePiece(capturedPawnSq);
}

void ChessBoard::handleCastling(int startSquare, int endSquare, Color color) {
    if (startSquare == E1 && endSquare == G1) {
        removePiece(H1);
        placePiece(F1, Piece(WHITE, ROOK));
    } else if (startSquare == E1 && endSquare == C1) {
        removePiece(A1);
        placePiece(D1, Piece(WHITE, ROOK));
    } else if (startSquare == E8 && endSquare == G8) {
        removePiece(H8);
        placePiece(F8, Piece(BLACK, ROOK));
    } else if (startSquare == E8 && endSquare == C8) {
        removePiece(A8);
        placePiece(D8, Piece(BLACK, ROOK));
    }
}

void ChessBoard::handlePromotion(int endSquare, Color color, PieceType promoteTo) {
    removePiece(endSquare);
    placePiece(endSquare, Piece(color, promoteTo));
}

// ============================================================================
// BOARD STATE MANAGEMENT
// ============================================================================

void ChessBoard::saveBoardState() {
    BoardState state;
    state.whiteBoards = whiteBoards;
    state.blackBoards = blackBoards;
    state.allWhitePieces = allWhitePieces;
    state.allBlackPieces = allBlackPieces;
    state.allPieces = allPieces;
    state.colorToMove = colorToMove;
    state.canWhiteCastleKingSide = canWhiteCastleKingSide;
    state.canBlackCastleKingSide = canBlackCastleKingSide;
    state.canWhiteCastleQueenSide = canWhiteCastleQueenSide;
    state.canBlackCastleQueenSide = canBlackCastleQueenSide;
    state.isThereCheck = isThereCheck;
    state.allSquaresAttackedByWhite = allSquaresAttackedByWhite;
    state.allSquaresAttackedByBlack = allSquaresAttackedByBlack;
    state.lastMove = lastMove;
    state.key = currentHash;
    boardStateStack.push(state);
}

void ChessBoard::restoreBoardState() {
    if (boardStateStack.empty()) return;
    
    BoardState state = boardStateStack.top();
    boardStateStack.pop();
    
    whiteBoards = state.whiteBoards;
    blackBoards = state.blackBoards;
    allWhitePieces = state.allWhitePieces;
    allBlackPieces = state.allBlackPieces;
    allPieces = state.allPieces;
    colorToMove = state.colorToMove;
    canWhiteCastleKingSide = state.canWhiteCastleKingSide;
    canBlackCastleKingSide = state.canBlackCastleKingSide;
    canWhiteCastleQueenSide = state.canWhiteCastleQueenSide;
    canBlackCastleQueenSide = state.canBlackCastleQueenSide;
    isThereCheck = state.isThereCheck;
    allSquaresAttackedByWhite = state.allSquaresAttackedByWhite;
    allSquaresAttackedByBlack = state.allSquaresAttackedByBlack;
    lastMove = state.lastMove;
    currentHash = state.key;
    
    repetitionTable[currentHash]--;
}

void ChessBoard::updateBoards() {
    allWhitePieces = whiteBoards[0] | whiteBoards[1] | whiteBoards[2] | 
                     whiteBoards[3] | whiteBoards[4] | whiteBoards[5];
    allBlackPieces = blackBoards[0] | blackBoards[1] | blackBoards[2] | 
                     blackBoards[3] | blackBoards[4] | blackBoards[5];
    allPieces = allWhitePieces | allBlackPieces;
    
    initializeAllSquaresAttackedByBlack();
    initializeAllSquaresAttackedByWhite();
}

bool ChessBoard::isThreefoldRepetition(const Move &move) const {
    auto it = repetitionTable.find(currentHash);
    return (it != repetitionTable.end() && it->second >= 3);
}

// ============================================================================
// ZOBRIST HASHING
// ============================================================================

void ChessBoard::initializeHashKeys() {
    for (int color = 0; color < 2; color++) {
        for (int piece = 0; piece < 6; piece++) {
            for (int square = 0; square < 64; square++) {
                zorbristTable[color][piece][square] = randomUInt64();
            }
        }
    }
    
    turnKey = randomUInt64();
    
    for (int i = 0; i < 4; i++) {
        castlingKeys[i] = randomUInt64();
    }
    
    for (int i = 0; i < 8; i++) {
        enPassantKeys[i] = randomUInt64();
    }
}

unsigned long long ChessBoard::generateInitialHash() const {
    unsigned long long hash = 0;
    
    for (int sq = 0; sq < 64; sq++) {
        auto piece = getPiece(sq);
        if (piece.type != EMPTY) {
            hash ^= zorbristTable[piece.color][piece.type][sq];
        }
    }
    
    if (colorToMove == WHITE) {
        hash ^= turnKey;
    }
    
    if (canWhiteCastleKingSide)  hash ^= castlingKeys[0];
    if (canWhiteCastleQueenSide) hash ^= castlingKeys[1];
    if (canBlackCastleKingSide)  hash ^= castlingKeys[2];
    if (canBlackCastleQueenSide) hash ^= castlingKeys[3];
    
    if (enPassantFile >= 0) {
        hash ^= enPassantKeys[enPassantFile];
    }
    
    return hash;
}

unsigned long long ChessBoard::updateHash(unsigned long long hash, const Move& move) const {
    hash ^= zorbristTable[move.movingPieceColor][move.movingPieceType][move.startingSquare];
    
    if (move.capturedPiece.type != EMPTY) {
        int capturedSq = move.enpassant ? lastMove.endSquare : move.endSquare;
        hash ^= zorbristTable[move.capturedPiece.color][move.capturedPiece.type][capturedSq];
    }
    
    if (move.isPromotion) {
        hash ^= zorbristTable[move.movingPieceColor][move.promotedPieceType][move.endSquare];
    } else {
        hash ^= zorbristTable[move.movingPieceColor][move.movingPieceType][move.endSquare];
    }
    
    if (enPassantFile >= 0) {
        hash ^= enPassantKeys[enPassantFile];
    }
    
    hash ^= turnKey;
    
    return hash;
}

// ============================================================================
// EVALUATION MASKS
// ============================================================================

void ChessBoard::initializeFileMasks() {
    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            int sq = rank * 8 + file;
            fileMasks[sq] = H_FILE << file;
        }
    }
}

void ChessBoard::initializeRankMasks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            rankMasks[sq] = BOTTOM_ROW << (8 * rank);
        }
    }
}

void ChessBoard::initializeIsolatedMasks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            
            if (file == 0) {
                isolatedMasks[sq] = fileMasks[sq + 1];
            } else if (file == 7) {
                isolatedMasks[sq] = fileMasks[sq - 1];
            } else {
                isolatedMasks[sq] = fileMasks[sq + 1] | fileMasks[sq - 1];
            }
        }
    }
}

void ChessBoard::initializeWhitePassedPawnMasks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            
            if (file == 0) {
                whitePassedMasks[sq] = fileMasks[sq + 1] | fileMasks[sq];
            } else if (file == 7) {
                whitePassedMasks[sq] = fileMasks[sq - 1] | fileMasks[sq];
            } else {
                whitePassedMasks[sq] = fileMasks[sq + 1] | fileMasks[sq - 1] | fileMasks[sq];
            }
            
            for (int r = 0; r <= rank; r++) {
                whitePassedMasks[sq] &= ~rankMasks[r * 8 + file];
            }
        }
    }
}

void ChessBoard::initializeBlackPassedPawnMasks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            
            if (file == 0) {
                blackPassedMasks[sq] = fileMasks[sq + 1] | fileMasks[sq];
            } else if (file == 7) {
                blackPassedMasks[sq] = fileMasks[sq - 1] | fileMasks[sq];
            } else {
                blackPassedMasks[sq] = fileMasks[sq + 1] | fileMasks[sq - 1] | fileMasks[sq];
            }
            
            for (int r = rank; r < 8; r++) {
                blackPassedMasks[sq] &= ~rankMasks[r * 8 + file];
            }
        }
    }
}

// ============================================================================
// FEN PARSING
// ============================================================================

void ChessBoard::setBoardFromFEN(const string& fen) {
    std::istringstream iss(fen);
    string boardPart, activeColor, castling, enPassant, halfmove, fullmove;
    iss >> boardPart >> activeColor >> castling >> enPassant >> halfmove >> fullmove;
    
    whiteBoards = vector<unsigned long long>(6, 0ULL);
    blackBoards = vector<unsigned long long>(6, 0ULL);
    allWhitePieces = 0ULL;
    allBlackPieces = 0ULL;
    allPieces = 0ULL;
    
    int sq = 63;
    for (char c : boardPart) {
        if (c == '/') {
            continue;
        } else if (isdigit(c)) {
            sq -= (c - '0');
        } else {
            Color color = isupper(c) ? WHITE : BLACK;
            PieceType type;
            
            switch (tolower(c)) {
                case 'p': type = PAWN; break;
                case 'r': type = ROOK; break;
                case 'n': type = KNIGHT; break;
                case 'b': type = BISHOP; break;
                case 'q': type = QUEEN; break;
                case 'k': type = KING; break;
                default:  type = EMPTY; break;
            }
            
            placePiece(sq, Piece(color, type));
            sq--;
        }
    }
    
    colorToMove = (activeColor == "w") ? WHITE : BLACK;
    
    canWhiteCastleKingSide  = (castling.find('K') != string::npos);
    canWhiteCastleQueenSide = (castling.find('Q') != string::npos);
    canBlackCastleKingSide  = (castling.find('k') != string::npos);
    canBlackCastleQueenSide = (castling.find('q') != string::npos);
    
    if (enPassant != "-") {
        enPassantFile = enPassant[0] - 'a';
    } else {
        enPassantFile = -1;
    }
    
    updateBoards();
    saveBoardState();
}

// ============================================================================
// PERFT
// ============================================================================

void ChessBoard::perft(int depth) {
    if (depth == 0) {
        nodes++;
        return;
    }
    
    vector<Move> moveList;
    generateMoves(colorToMove, moveList);
    
    for (auto& move : moveList) {
        saveBoardState();
        if (makeAMove(move)) {
            perft(depth - 1);
            undoMove();
        }
        restoreBoardState();
    }
}

void ChessBoard::perftTest(int depth, int initialDepth) {
    if (depth == 0) {
        nodes++;
        return;
    }
    
    vector<Move> moveList;
    generateMoves(colorToMove, moveList);
    
    for (auto& move : moveList) {
        saveBoardState();
        if (!makeAMove(move)) continue;
        
        perftTest(depth - 1, initialDepth);
        undoMove();
    }
}

// ============================================================================
// DISPLAY
// ============================================================================

void ChessBoard::printChessBoard() {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 7; file >= 0; file--) {
            int sq = rank * 8 + file;
            Piece piece = getPiece(sq);
            
            if (piece.type != EMPTY) {
                cout << pieceToChar(piece) << "  ";
            } else {
                cout << ".  ";
            }
        }
        cout << "\n";
    }
    cout << "\nA  B  C  D  E  F  G  H\n\n";
}

string ChessBoard::indexToNotation(int index) {
    string notation;
    notation += 'a' + 7 - (index % 8);
    notation += '1' + (index / 8);
    return notation;
}

void ChessBoard::printMove(int startSquare, int endSquare) {
    cout << indexToNotation(startSquare) << indexToNotation(endSquare);
}

char ChessBoard::pieceToChar(Piece piece) {
    switch (piece.type) {
        case PAWN:   return (piece.color == BLACK) ? 'p' : 'P';
        case KNIGHT: return (piece.color == BLACK) ? 'n' : 'N';
        case BISHOP: return (piece.color == BLACK) ? 'b' : 'B';
        case ROOK:   return (piece.color == BLACK) ? 'r' : 'R';
        case QUEEN:  return (piece.color == BLACK) ? 'q' : 'Q';
        case KING:   return (piece.color == BLACK) ? 'k' : 'K';
        default:     return '.';
    }
}