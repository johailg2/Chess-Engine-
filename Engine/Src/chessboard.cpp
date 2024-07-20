#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include<iterator>
#include <conio.h> // for _getch()
#include <random>
#include <sstream>
#include "../include/chessboard.h"
#include "../include/attackTables.h"
#include "../include/bitboards.h"

    ChessBoard::ChessBoard() {
    // White pieces positions
    auto whitePawns = 0b0000000000000000000000000000000000000000000000001111111100000000ULL;
    auto whiteRooks = 0b0000000000000000000000000000000000000000000000000000000010000001ULL;
    auto whiteKnights = 0b0000000000000000000000000000000000000000000000000000000001000010ULL;
    auto whiteBishops = 0b0000000000000000000000000000000000000000000000000000000000100100ULL;
    auto whiteQueen = 0b0000000000000000000000000000000000000000000000000000000000010000ULL; // d1
    auto whiteKing = 0b0000000000000000000000000000000000000000000000000000000000001000ULL;  // e1

    // Black pieces positions
    auto blackPawns = 0b0000000011111111000000000000000000000000000000000000000000000000ULL;
    auto blackRooks = 0b1000000100000000000000000000000000000000000000000000000000000000ULL;
    auto blackKnights = 0b0100001000000000000000000000000000000000000000000000000000000000ULL;
    auto blackBishops = 0b0010010000000000000000000000000000000000000000000000000000000000ULL;
    auto blackQueen = 0b0001000000000000000000000000000000000000000000000000000000000000ULL; // d8
    auto blackKing = 0b0000100000000000000000000000000000000000000000000000000000000000ULL;  // e8

    // Collective positions
    allWhitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueen | whiteKing;
    allBlackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueen | blackKing;
    allPieces = allWhitePieces | allBlackPieces;

    colorToMove = WHITE;
    bool canWhiteCastleKingSide = true;
    bool canBlackCastleKingSide = true;

    bool canWhiteCastleQueenSide = true;
    bool canBlackCastleQueenSide = true;

    bool isThereCheck = false;
    whiteBoards = {whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing};
    blackBoards = {blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing};
    intializeAllSquaresAttackedByBlack();
    intializeAllSquaresAttackedByWhite();
}

void ChessBoard::initialize(){
    initializeAllSlidingMasks();
    initializeAttackTables(1);
    initializeAttackTables(0);
    initializeHashKeys();
    currentHash = generateInitialHash();

}

unsigned long long ChessBoard::getPieceBoard(Piece piece) const
{
    switch (piece.type)
    {
    case PAWN:
        if(piece.color == WHITE){
            return whiteBoards[0];
        }
        return blackBoards[0];

    case ROOK:
        if(piece.color == WHITE){
            return whiteBoards[1];
        } 
        return blackBoards[1];    

    case BISHOP:
        if(piece.color == WHITE){
            return whiteBoards[3];
        }
        return blackBoards[3];  

    case KNIGHT:
        if(piece.color == WHITE){
            return whiteBoards[2];
        }
        return blackBoards[2];  

    case QUEEN:
        if(piece.color == WHITE){
            return whiteBoards[4];
        }
        return blackBoards[4];   

    case KING:
        if(piece.color == WHITE){
            return whiteBoards[5];
        }
        return blackBoards[5];
    }
    return 0ULL;
}

Piece ChessBoard::getPiece(int square) const {
    for (int i = 0; i < 6; ++i) {
        if (whiteBoards[i] & (1ULL << square)) {
            return Piece(WHITE, static_cast<PieceType>(i));
        }
        if (blackBoards[i] & (1ULL << square)) {
            return Piece(BLACK, static_cast<PieceType>(i));
        }
    }
    return Piece();
}

void ChessBoard::setPieceBoard(Piece piece, unsigned long long newBoard)
{
    switch (piece.type)
    {
    case PAWN:
        if(piece.color == WHITE){
            whiteBoards[0] = newBoard;
        } else {
            blackBoards[0] = newBoard;
        }
        break;
    case ROOK:
        if(piece.color == WHITE){
            whiteBoards[1] = newBoard;
        } else {
            blackBoards[1] = newBoard;
        }
        break;
    case BISHOP:
        if(piece.color == WHITE){
            whiteBoards[3] = newBoard;
        } else {
            blackBoards[3] = newBoard;
        }
        break;
    case KNIGHT:
        if(piece.color == WHITE){
            whiteBoards[2] = newBoard;
        } else {
            blackBoards[2] = newBoard;
        }
        break;
    case QUEEN:
        if(piece.color == WHITE){
            whiteBoards[4] = newBoard;
        } else {
            blackBoards[4] = newBoard;
        }
        break;
    case KING:
        if(piece.color == WHITE){
            whiteBoards[5] = newBoard;
        } else {
            blackBoards[5] = newBoard;
        }
        break;
    }
}

unsigned long long ChessBoard::pawnAttacks(int square, Color color)
{
    unsigned long long attackBoard = 0ULL;
    BitBoard pawnBoard;
    pawnBoard.setBit(square);
    if (color == WHITE){
        attackBoard |= (pawnBoard.getBoard() << 7) & NOT_A_FILE;
        attackBoard |= (pawnBoard.getBoard() << 9) & NOT_H_FILE;
    }
    else{
        attackBoard |= (pawnBoard.getBoard() >> 7) & NOT_H_FILE;
        attackBoard |= (pawnBoard.getBoard() >> 9) & NOT_A_FILE;
    }
    return attackBoard;
}


void ChessBoard::populatePawnCaptures(Color color, Move lastMove, vector<Move>& moveList) {
    BitBoard pawnBoard = (color == WHITE) ? BitBoard(whiteBoards[0]) : BitBoard(blackBoards[0]);

    if (lastMove.doublePawn) {
        int direction = (color == WHITE) ? 1 : -1; 
        int rankTrigger = (color == WHITE) ? 4 : 3; 

        if (lastMove.endSquare / 8 == rankTrigger) {
            for (int offset : {-1, 1}) { 
                int neighborSquare = lastMove.endSquare + offset;
                if (neighborSquare >= 0 && neighborSquare < 64 && (pawnBoard.getBoard() & (1ULL << neighborSquare))) {
                    if ((lastMove.endSquare % 8 != 0 || neighborSquare % 8 != 7) && (lastMove.endSquare % 8 != 7 || neighborSquare % 8 != 0)) {
                        int enPassantSquare = lastMove.endSquare + (direction * 8);
                        moveList.emplace_back(Move(neighborSquare, enPassantSquare, PAWN, color, getPiece(lastMove.endSquare), EMPTY, 0, 0, 1, 0));
                    }
                }
            }
        }
    }

    while (pawnBoard.getBoard()) {
        int currentSquare = pawnBoard.LS1B();
        pawnBoard.flipBit(currentSquare);
        BitBoard attackBoard = BitBoard(pawnAttacks(currentSquare, color));
        attackBoard.setBoard(attackBoard.getBoard() & getOpposingBoard(color));
        while (attackBoard.getBoard()) {
            int endSquare = attackBoard.LS1B();
            attackBoard.flipBit(endSquare);
            if (endSquare >= 0 && endSquare < 64 && ((currentSquare % 8) != 0 || (endSquare % 8) != 7) && ((currentSquare % 8) != 7 || (endSquare % 8) != 0)) {
                if (endSquare / 8 == (color == WHITE ? 7 : 0)) {
                    PieceType promotionTypes[] = {ROOK, KNIGHT, BISHOP, QUEEN};
                    for (PieceType promotionType : promotionTypes) {
                        moveList.emplace_back(Move(currentSquare, endSquare, PAWN, color, getPiece(endSquare), promotionType, 1, 0, 0, 0));
                    }
                } else {
                    moveList.emplace_back(Move(currentSquare, endSquare, PAWN, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
                }
            }
        }
    }
}

void ChessBoard::populateQuietPawnMoves(Color color, vector<Move>& moveList) {
    BitBoard pawnBoard = (color == WHITE) ? BitBoard(whiteBoards[0]) : BitBoard(blackBoards[0]);

    while (pawnBoard.getBoard()) {
        int currentSquare = pawnBoard.LS1B();
        pawnBoard.flipBit(currentSquare);

        int singlePushSquare = currentSquare + (color == WHITE ? 8 : -8);
        if (singlePushSquare >= 0 && singlePushSquare < 64) {
            if (!(allPieces & (1ULL << singlePushSquare))) { 
                int targetRow = (singlePushSquare / 8); 
                if (targetRow == (color == WHITE ? 7 : 0)) {  
                    PieceType promotionTypes[] = {ROOK, KNIGHT, BISHOP, QUEEN};
                    for (PieceType promotionType : promotionTypes) {
                        moveList.emplace_back(Move(currentSquare, singlePushSquare, PAWN, color, Piece(), promotionType, 1, 0, 0, 0));
                    }
                } else {
                    moveList.emplace_back(Move(currentSquare, singlePushSquare, PAWN, color, Piece(), EMPTY, 0, 0, 0, 0));
                }

                // Double push
                if ((color == WHITE && currentSquare / 8 == 1) || (color == BLACK && currentSquare / 8 == 6)) {
                    int doublePushSquare = currentSquare + (color == WHITE ? 16 : -16);
                    singlePushSquare = currentSquare + (color == WHITE ? 8 : -8);
                    if (doublePushSquare >= 0 && doublePushSquare < 64) {
                        if (!(allPieces & (1ULL << doublePushSquare) || (allPieces & (1ULL << singlePushSquare)))) {  // Check if both squares ahead are clear
                            moveList.emplace_back(Move(currentSquare, doublePushSquare, PAWN, color, Piece(), EMPTY, 0, 1, 0, 0));
                            int leftSquare = doublePushSquare - 1;
                            int rightSquare = doublePushSquare + 1;
                            auto enemyPawnBoard = (color != WHITE) ? BitBoard(whiteBoards[0]) : BitBoard(blackBoards[0]);
                            if(enemyPawnBoard.getBit(leftSquare) || enemyPawnBoard.getBit(rightSquare)){
                                enPassantEnable.emplace_back(Move(currentSquare, doublePushSquare, PAWN, color, Piece(), EMPTY, 0, 1, 0, 0));
                            }
                        }
                    }
                }
            }
        }
    }
}


unsigned long long ChessBoard::generateKnightAttacks(int square){
    unsigned long long attackBoard = 0ULL;
    BitBoard knightBoard;
    knightBoard.setBit(square);

    attackBoard |= (knightBoard.getBoard() << 17) & NOT_H_FILE;
    attackBoard |= (knightBoard.getBoard() << 15) & NOT_A_FILE;
    attackBoard |= (knightBoard.getBoard() << 10) & NOT_GH_FILE;
    attackBoard |= (knightBoard.getBoard() << 6) & NOT_AB_FILE;

    attackBoard |= (knightBoard.getBoard() >> 17) & NOT_A_FILE;
    attackBoard |= (knightBoard.getBoard() >> 15) & NOT_H_FILE;
    attackBoard |= (knightBoard.getBoard() >> 10) & NOT_AB_FILE;
    attackBoard |= (knightBoard.getBoard() >> 6) & NOT_GH_FILE;

    return attackBoard;
}

void ChessBoard::populateKnightMoves(Color color, vector<Move>& moveList, bool captureOnly)
{
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard knightBoard = BitBoard(getPieceBoard(Piece(color, KNIGHT)));
    unsigned long long attackBoard;
    BitBoard captureBoard;
    BitBoard quietBoard;

    while(knightBoard.getBoard()){
        int currentSquare = knightBoard.LS1B();
        knightBoard.flipBit(currentSquare);
        attackBoard = (generateKnightAttacks(currentSquare));
        captureBoard = BitBoard(attackBoard & opposingBoard);
        while(captureBoard.getBoard()){
            int endSquare = captureBoard.LS1B();
            captureBoard.flipBit(endSquare);
            moveList.emplace_back(Move(currentSquare, endSquare, KNIGHT, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }

        if(captureOnly){
            continue;
        }

        auto quietBoard = BitBoard((attackBoard & ~friendlyBoard) & ~opposingBoard);
        while(quietBoard.getBoard()){
            int endSquare = quietBoard.LS1B();
            quietBoard.flipBit(endSquare);
            moveList.emplace_back(Move(currentSquare, endSquare, KNIGHT, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }
    }

}

unsigned long long ChessBoard::generateKingAttacks(int square){
    unsigned long long attackBoard = 0ULL;
    BitBoard kingBoard;
    kingBoard.setBit(square);

    attackBoard |= (kingBoard.getBoard() >> 8);
    attackBoard |= (kingBoard.getBoard() >> 9) & NOT_A_FILE;
    attackBoard |= (kingBoard.getBoard() >> 7) & NOT_H_FILE;

    attackBoard |= (kingBoard.getBoard() << 8);
    attackBoard |= (kingBoard.getBoard() << 9) & NOT_H_FILE;
    attackBoard |= (kingBoard.getBoard() << 7) & NOT_A_FILE;

    attackBoard |= (kingBoard.getBoard() >> 1) & NOT_A_FILE;
    attackBoard |= (kingBoard.getBoard() << 1) & NOT_H_FILE;

    return attackBoard;
}

void ChessBoard::populateKingMoves(Color color, vector<Move>& moveList, bool captureOnly) {
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard kingBoard = BitBoard((color == WHITE) ? whiteBoards[5] : blackBoards[5]);
    int kingSquare = kingBoard.LS1B();
    unsigned long long attackBoard = generateKingAttacks(kingSquare);
    BitBoard captureBoard;

    captureBoard = BitBoard(attackBoard & opposingBoard);
    while (captureBoard.getBoard()) {
        int endSquare = captureBoard.LS1B();
        captureBoard.flipBit(endSquare);
        if (isSquareAttacked(endSquare, (color == WHITE) ? WHITE : BLACK)) continue;
            moveList.emplace_back(Move(kingSquare, endSquare, KING, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
    }

    if(captureOnly){
        return;
    }
    // Check for castling
    if (color == WHITE) {
        if (canWhiteCastleQueenSide && !(allPieces & ((1ULL << (kingSquare + 1)) | (1ULL << (kingSquare + 2)))) &&
            !isSquareAttacked(kingSquare, WHITE) && !isSquareAttacked(kingSquare + 1, WHITE) && !isSquareAttacked(kingSquare + 2, WHITE)) {
                moveList.emplace_back(Move(kingSquare, kingSquare + 2, KING, WHITE, Piece(), EMPTY, 0, 0, 0, 1));
            }
                        
        if (canWhiteCastleKingSide && !(allPieces & ((1ULL << (kingSquare - 1)) | (1ULL << (kingSquare - 2)))) &&
            !isSquareAttacked(kingSquare, WHITE) && !isSquareAttacked(kingSquare - 1, WHITE) && !isSquareAttacked(kingSquare - 2, WHITE)) {
            moveList.emplace_back(Move(kingSquare, kingSquare - 2, KING, WHITE, Piece(), EMPTY, 0, 0, 0, 1));
        }
    } else {
        if (canBlackCastleKingSide && !(allPieces & ((1ULL << (kingSquare - 1)) | (1ULL << (kingSquare - 2)))) &&
            !isSquareAttacked(kingSquare, BLACK) && !isSquareAttacked(kingSquare - 1, BLACK) && !isSquareAttacked(kingSquare - 2, BLACK)) {
            moveList.emplace_back(Move(kingSquare, kingSquare - 2, KING, BLACK, Piece(), EMPTY, 0, 0, 0, 1));
        }
        if (canBlackCastleQueenSide && !(allPieces & ((1ULL << (kingSquare + 1)) | (1ULL << (kingSquare + 2)))) &&
            !isSquareAttacked(kingSquare, BLACK) && !isSquareAttacked(kingSquare + 1, BLACK) && !isSquareAttacked(kingSquare + 2, BLACK)) {
            moveList.emplace_back(Move(kingSquare, kingSquare + 2, KING, BLACK, Piece(), EMPTY, 0, 0, 0, 1));
        }
    }

    auto quietBoard = BitBoard((attackBoard & ~friendlyBoard) & ~opposingBoard);
    while (quietBoard.getBoard()) {
        int endSquare = quietBoard.LS1B();
        quietBoard.flipBit(endSquare);
        if (isSquareAttacked(endSquare, (color == WHITE) ? WHITE : BLACK)) continue;
        moveList.emplace_back(Move(kingSquare, endSquare, KING, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
    }
}

unsigned long long ChessBoard::bishopAttackMasks(int square){
    unsigned long long attackBoard = 0ULL;
    int movingRow = square / 8;
    int movingCol = square % 8;

    for (int currentRow = movingRow + 1, currentCol = movingCol + 1; currentRow < 7 && currentCol < 7; currentRow++, currentCol++){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
    }

    for (int currentRow = movingRow + 1, currentCol = movingCol - 1; currentRow < 7 && currentCol > 0; currentRow++, currentCol--){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
    }

    for (int currentRow = movingRow - 1, currentCol = movingCol + 1; currentRow > 0 && currentCol < 7; currentRow--, currentCol++){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
    }

    for (int currentRow = movingRow - 1, currentCol = movingCol - 1; currentRow > 0 && currentCol > 0; currentRow--, currentCol--){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
    }

    return attackBoard;
}

unsigned long long ChessBoard::bishopRestrictedAttackMasks(int square, unsigned long long restOfBoard){
    unsigned long long attackBoard = 0ULL;
    int movingRow = square / 8;
    int movingCol = square % 8;

    for (int currentRow = movingRow + 1, currentCol = movingCol + 1; currentRow < 8 && currentCol < 8; currentRow++, currentCol++){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
        if (1ULL << (currentRow * 8 + currentCol) & restOfBoard) break;
    }

    for (int currentRow = movingRow + 1, currentCol = movingCol - 1; currentRow < 8 && currentCol >= 0; currentRow++, currentCol--){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
        if (1ULL << (currentRow * 8 + currentCol) & restOfBoard) break;
    }

    for (int currentRow = movingRow - 1, currentCol = movingCol + 1; currentRow >= 0 && currentCol < 8; currentRow--, currentCol++){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
        if (1ULL << (currentRow * 8 + currentCol) & restOfBoard) break;
    }

    for (int currentRow = movingRow - 1, currentCol = movingCol - 1; currentRow >= 0 && currentCol >= 0; currentRow--, currentCol--){
        attackBoard |= 1ULL << (currentRow * 8 + currentCol);
        if (1ULL << (currentRow * 8 + currentCol) & restOfBoard) break;
    }

    return attackBoard;
}

void ChessBoard::populateBishopMoves(Color color, vector<Move>& moveList, bool captureOnly)
{
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard bishopBoard = BitBoard(getPieceBoard(Piece(color, BISHOP)));
    unsigned long long attackBoard;
    BitBoard captureBoard;

    while(bishopBoard.getBoard()){
        int currentSquare = bishopBoard.LS1B();
        bishopBoard.flipBit(currentSquare);
        attackBoard = instantBishopAttacks(getAllPieces(), currentSquare);
        captureBoard = BitBoard(attackBoard & opposingBoard);
        while(captureBoard.getBoard()){
            int endSquare = captureBoard.LS1B();
            captureBoard.flipBit(endSquare);
            moveList.emplace_back(Move(currentSquare, endSquare, BISHOP, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }
        
        if(captureOnly){
            continue;
        }

        auto quietBoard = BitBoard((attackBoard & ~friendlyBoard) & ~opposingBoard);
        while(quietBoard.getBoard()){
            int endSquare = quietBoard.LS1B();
            quietBoard.flipBit(endSquare);
            moveList.emplace_back(Move(currentSquare, endSquare, BISHOP, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }
    }
}

unsigned long long ChessBoard::rookAttackMasks(int square){
    unsigned long long attackBoard = 0ULL;
    int movingRow = square / 8;
    int movingCol = square % 8;

    for (int currentRow = movingRow + 1; currentRow < 7; currentRow++){
        attackBoard |= 1ULL << (currentRow * 8 + movingCol);
    }

    for (int currentCol = movingCol - 1; currentCol > 0; currentCol--){
        attackBoard |= 1ULL << (movingRow * 8 + currentCol);
    }

    for (int currentCol = movingCol + 1; currentCol < 7; currentCol++){
        attackBoard |= 1ULL << (movingRow * 8 + currentCol);
    }

    for (int currentRow = movingRow - 1; currentRow > 0; currentRow--){
        attackBoard |= 1ULL << (currentRow * 8 + movingCol);
    }

    return attackBoard;
}

unsigned long long ChessBoard::rookRestrictedAttacksMasks(int square, unsigned long long restOfBoard){
    unsigned long long attackBoard = 0ULL;
    int movingRow = square / 8;
    int movingCol = square % 8;

    for (int currentRow = movingRow + 1; currentRow < 8; currentRow++){
        attackBoard |= 1ULL << (currentRow * 8 + movingCol);
        if (1ULL << (currentRow * 8 + movingCol) & restOfBoard) break;
    }

    for (int currentCol = movingCol - 1; currentCol >= 0; currentCol--){
        attackBoard |= 1ULL << (movingRow * 8 + currentCol);
        if (1ULL << (movingRow * 8 + currentCol) & restOfBoard) break;
    }

    for (int currentCol = movingCol + 1; currentCol < 8; currentCol++){
        attackBoard |= 1ULL << (movingRow * 8 + currentCol);
        if (1ULL << (movingRow * 8 + currentCol) & restOfBoard) break;
    }

    for (int currentRow = movingRow - 1; currentRow >= 0; currentRow--){
        attackBoard |= 1ULL << (currentRow * 8 + movingCol);
        if (1ULL << (currentRow * 8 + movingCol) & restOfBoard) break;
    }

    return attackBoard;
}

void ChessBoard::populateRookMoves(Color color, vector<Move>& moveList, bool captureOnly)
{
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard rookBoard = BitBoard(getPieceBoard(Piece(color, ROOK)));
    unsigned long long attackBoard;
    BitBoard captureBoard;

    while(rookBoard.getBoard()){
        int currentSquare = rookBoard.LS1B();
        rookBoard.flipBit(currentSquare);
        attackBoard = instantRookAttacks(getAllPieces(), currentSquare);
        captureBoard = BitBoard(attackBoard & opposingBoard);
        while(captureBoard.getBoard()){
            int endSquare = captureBoard.LS1B();
            captureBoard.flipBit(endSquare);
            moveList.emplace_back(Move(currentSquare, endSquare, ROOK, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }

        if(captureOnly){
            continue;
        }

        auto quietBoard = BitBoard((attackBoard & ~friendlyBoard) & ~opposingBoard);
        while(quietBoard.getBoard()){
            int endSquare = quietBoard.LS1B();
            quietBoard.flipBit(endSquare);
            moveList.emplace_back(Move(currentSquare, endSquare, ROOK, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }
        
    }
}

unsigned long long ChessBoard::queenAttackMasks(int square){
    auto rookMask = rookAttackMasks(square);
    auto bishopMask = bishopAttackMasks(square);
    return rookMask | bishopMask;
}

unsigned long long ChessBoard::queenRestrictedAttackMasks(int square, unsigned long long restOfBoard){
    auto rookMask = rookRestrictedAttacksMasks(square, restOfBoard);
    auto bishopMask = bishopRestrictedAttackMasks(square, restOfBoard);
    return rookMask | bishopMask;
}

void ChessBoard::populateQueenMoves(Color color, vector<Move>& moveList, bool captureOnly)
{
    unsigned long long friendlyBoard = getFriendlyBoard(color);
    unsigned long long opposingBoard = getOpposingBoard(color);
    BitBoard queenBoard = BitBoard(getPieceBoard(Piece(color, QUEEN)));
    unsigned long long attackBoard;
    BitBoard captureBoard;

    while(queenBoard.getBoard()){
        int currentSquare = queenBoard.LS1B();
        queenBoard.flipBit(currentSquare);
        attackBoard = instantQueenAttacks(getAllPieces(), currentSquare);
        captureBoard = BitBoard(attackBoard & opposingBoard);
        while(captureBoard.getBoard()){
            int endSquare = captureBoard.LS1B();
            captureBoard.flipBit(endSquare);
            moveList.emplace_back(Move(currentSquare, endSquare, QUEEN, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }

        if(captureOnly){
            return;
        }

        auto quietBoard = BitBoard((attackBoard & ~friendlyBoard) & ~opposingBoard);
        while(quietBoard.getBoard()){
            int endSquare = quietBoard.LS1B();
            quietBoard.flipBit(endSquare);
             moveList.emplace_back(Move(currentSquare, endSquare, QUEEN, color, getPiece(endSquare), EMPTY, 0, 0, 0, 0));
        }
    }
}

/*this is a really cool way of encoding all the possible occupancy positions.
 A given attack mask is represented by U64 int. This int has a set number of 1s which represent the squares being 
 attacked with the rest being set to zero. A piece is attacked iff its bitboard has ones in the same set indices 
 as the attack mask. (as shown in restricted attack mask fnctn) So all possible occupancy positions can be counted 
 from zero up to (2^n) - 1 where n is the number of set bits in the attack mask. The counting works by starting at the
  LS1B and using that as a zero index for counting. The 2nd LS1B is used as the 1 index, the 3rd LS1B is used as the 2nd 
  and so on. Using this, we can attach an index to all possible occupancy positions for a given attack mask.
 The function below does the reverse, given an index, it produces the occupancy bitboard. */

unsigned long long attackMaskToOccupancy(int indexOfOccupancyConfig, int numOfSetBits, BitBoard attackMask){
    unsigned long long occupancy = 0ULL;
    
    for (int currentSetBitIndex = 0; currentSetBitIndex < numOfSetBits; currentSetBitIndex++){
        int currentLeastSigBit = attackMask.LS1B(); //the index of the least sig set bit
        attackMask.flipBit(currentLeastSigBit); //flip it since we dont have to deal with it anymore

        if (indexOfOccupancyConfig & (1 << currentSetBitIndex)){ //check if the inputted indexed occupancy has a bit set in the current bit index
            occupancy |= (1ULL << currentLeastSigBit);
        }
    }
    return occupancy;
}

unsigned long long randomUInt64(){
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<unsigned long long> dis;
    return dis(gen);
}

unsigned long long randomFewBits(){
    return randomUInt64() & randomUInt64() & randomUInt64();
}

unsigned long long ChessBoard::findMagicNumber(int square, int relevantBits, bool bishop){
    //declare all variables
    unsigned long long mask = bishop ? bishopAttackMasks(square) : rookAttackMasks(square);
    unsigned long long attackSet[4096], usedAttacks[4096], occupancySet[4096];
    unsigned long long magic;   
    int i, j, k, fail;
    int occupancyIndex = 1 << relevantBits;

    for (i = 0; i < occupancyIndex; i++){  
        occupancySet[i] = attackMaskToOccupancy(i, relevantBits, mask); //initialize all possible occupancies for a given piece at a given square
        attackSet[i] = bishop ? bishopRestrictedAttackMasks(square, occupancySet[i]) : rookRestrictedAttacksMasks(square, occupancySet[i]); //initialize all possible attacks given all possible occupancy sets
    }

    for (k = 0; k < 100000000; k++){ //certain number of tries
        magic = randomFewBits(); //randomize a number to try
        if (__builtin_popcountll((mask * magic) & 0xFF00000000000000ULL) < 6) continue; //If the top 8 bits of the product have fewer than 6 set bits, 
                                                                                        //it indicates that the distribution is likely too sparse, and thus, the magic number candidate is not suitable.

        std::fill(std::begin(usedAttacks), std::end(usedAttacks), 0ULL); //initialize used attacks ot 0 
        for (i = 0, fail = 0; !fail && i < occupancyIndex; i++){
            j = (int)((occupancySet[i] * magic) >> (64 - relevantBits)); //this is the hash function that maps occupancies to attack sets (the magic!)
            if (usedAttacks[j] == 0ULL){
                usedAttacks[j] = attackSet[i]; //if the attack hasnt been used set it
            }
            else if (usedAttacks[j] != attackSet[i]){ //if there is a collision leave and try again
                fail = 1;
            }
        }
        if (!fail) return magic;
    }
    std::cout << "***Failed***" << std::endl;
    return 0ULL;
}

void ChessBoard::initializeAllSlidingMasks()
{
    for(int i = 0; i < 64; i++){
        allBishopMasks[i] = bishopAttackMasks(i);
        allRookMasks[i] = rookAttackMasks(i);
    }
}

void ChessBoard::initializeAttackTables(int bishop)
{  
    unsigned long long attackMask;
    for(int square = 0; square < 64; square++){
        if(bishop){
            attackMask = allBishopMasks[square]; //get the relevant attack mask for the current square
        }
        else{
            attackMask = allRookMasks[square];
        }
        int relevantBits = __builtin_popcountll(attackMask); //calc number of bits
        int occupancyIndices = 1 << relevantBits; //left shift to the number of total possible occupancy positions

        for(int index = 0; index < occupancyIndices; index++){
            auto occupancyForCurrentMask = attackMaskToOccupancy(index, relevantBits, attackMask); //iterate over all possible occupancies

            if(bishop){
                int magicIndex = (occupancyForCurrentMask*bishopMagics[square]) >> (64 - relevantBits); //get the magic Index using the magic number for the current occupancy
                magicBishopAttacks[square][magicIndex] = bishopRestrictedAttackMasks(square, occupancyForCurrentMask); //fill the attack table
            }

            else{
                int magicIndex = (occupancyForCurrentMask*rookMagics[square]) >> (64 - relevantBits);
                magicRookAttacks[square][magicIndex] = rookRestrictedAttacksMasks(square, occupancyForCurrentMask);
            }
        }
    }
}

unsigned long long ChessBoard::getAllSquaresAttackedByWhite()
{
    return allSquaresAttackedByWhite;
}

unsigned long long ChessBoard::getAllSquaresAttackedByBlack()
{
    return allSquaresAttackedByBlack;
}

unsigned long long ChessBoard::instantBishopAttacks(unsigned long long occupancy, int square)
{
    
    occupancy &= allBishopMasks[square];
    int magicIndex = (occupancy*bishopMagics[square]) >> (64 - __builtin_popcountll(allBishopMasks[square]));
    return magicBishopAttacks[square][magicIndex];
}

unsigned long long ChessBoard::instantRookAttacks(unsigned long long occupancy, int square)
{
    occupancy &= allRookMasks[square];
    int magicIndex = (occupancy*rookMagics[square]) >> (64 - __builtin_popcountll(allRookMasks[square]));
    return magicRookAttacks[square][magicIndex];
}

unsigned long long ChessBoard::instantQueenAttacks(unsigned long long occupancy, int square)
{
    return (instantBishopAttacks(occupancy, square) | instantRookAttacks(occupancy, square));
}

void ChessBoard::intializeAllSquaresAttackedByWhite()
{
    allSquaresAttackedByWhite = 0ULL;  

    BitBoard pieces(whiteBoards[0]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByWhite |= pawnAttacks(square, WHITE);
        pieces.flipBit(square);
    }

    pieces.setBoard(whiteBoards[2]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByWhite |= generateKnightAttacks(square);
        pieces.flipBit(square);
    }

    pieces.setBoard(whiteBoards[3]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByWhite |= instantBishopAttacks(getAllPieces(), square);
        pieces.flipBit(square);
    }

    pieces.setBoard(whiteBoards[1]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByWhite |= instantRookAttacks(getAllPieces(), square);
        pieces.flipBit(square);
    }

    pieces.setBoard(whiteBoards[4]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByWhite |= instantQueenAttacks(getAllPieces(), square);
        pieces.flipBit(square);
    }

    pieces.setBoard(whiteBoards[5]);
    if (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByWhite |= generateKingAttacks(square);
    }
}

void ChessBoard::intializeAllSquaresAttackedByBlack()
{
    allSquaresAttackedByBlack = 0ULL;  

    BitBoard pieces(blackBoards[0]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByBlack |= pawnAttacks(square, BLACK);
        pieces.flipBit(square);
    }

    pieces.setBoard(blackBoards[2]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByBlack |= generateKnightAttacks(square);
        pieces.flipBit(square);
    }

    pieces.setBoard(blackBoards[3]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByBlack |= instantBishopAttacks(getAllPieces(), square);
        pieces.flipBit(square);
    }

    pieces.setBoard(blackBoards[1]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByBlack |= instantRookAttacks(getAllPieces(), square);
        pieces.flipBit(square);
    }

    pieces.setBoard(blackBoards[4]);
    while (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByBlack |= instantQueenAttacks(getAllPieces(), square);
        pieces.flipBit(square);
    }

    pieces.setBoard(blackBoards[5]);
    if (pieces.getBoard()) {
        int square = pieces.LS1B();
        allSquaresAttackedByBlack |= generateKingAttacks(square);
    }
}

bool ChessBoard::isSquareAttacked(int square, Color color)
{
    BitBoard board = 0ULL;
    board.setBit(square);
    if (color == WHITE) {
        return (board.getBoard() & allSquaresAttackedByBlack);
    } else {
        return (board.getBoard() & allSquaresAttackedByWhite);
    }
}

void ChessBoard::generateMoves(Color color, vector<Move>& moveList) {
    moveList.clear();
    populatePawnCaptures(color, lastMove, moveList);
    populateQuietPawnMoves(color, moveList);
    populateKnightMoves(color, moveList,0);
    populateBishopMoves(color, moveList,0);
    populateQueenMoves(color, moveList,0);
    populateRookMoves(color, moveList,0);
    populateKingMoves(color, moveList,0);
}

void ChessBoard::generateCaptureMoves(Color color, vector<Move>& moveList){
    moveList.clear();
    populatePawnCaptures(color, lastMove, moveList);
    populateKnightMoves(color, moveList, 1);
    populateBishopMoves(color, moveList, 1);
    populateQueenMoves(color, moveList, 1);
    populateRookMoves(color, moveList, 1);
    populateKingMoves(color, moveList, 1);

}

bool ChessBoard::makeAMove(Move &move) {

    currentHash = updateHash(currentHash, move);
    removePiece(move.endSquare);
    removePiece(move.startingSquare);
    if(move.isPromotion){
        placePiece(move.endSquare, Piece(move.movingPieceColor, move.promotedPieceType));
    }
    else{
        placePiece(move.endSquare, Piece(move.movingPieceColor, move.movingPieceType));
    }
    
    if(move.movingPieceType == KING){
        if(move.movingPieceColor == WHITE){
            canWhiteCastleKingSide = 0;
            canWhiteCastleQueenSide = 0;
        }
        else{
            canBlackCastleKingSide = 0;
            canBlackCastleQueenSide = 0;
        }
    }  

    enPassantFile = -1;

    if(enPassantEnable.size()){
        for(auto m : enPassantEnable){
            if(m == move){
                enPassantFile = move.endSquare % 8;
            }
        }
    }

    if(move.movingPieceType == ROOK){
        if(move.startingSquare == 56){
            canBlackCastleKingSide = 0;
        }
        else if(move.startingSquare == 63){
            canBlackCastleQueenSide = 0;
        }
        else if(move.startingSquare == 0){
            canWhiteCastleKingSide = 0;
        }
        else if(move.startingSquare == 7){
            canWhiteCastleQueenSide = 0;
        }
    }

    if (move.capturedPiece.type == ROOK) {
        if (move.endSquare == 56) {
            canBlackCastleKingSide = 0;
        } 
        else if (move.endSquare == 63) {
            canBlackCastleQueenSide = 0;
        } 
        else if (move.endSquare == 0) {
            canWhiteCastleKingSide = 0;
        } 
        else if (move.endSquare == 7) {
            canWhiteCastleQueenSide = 0;
        }
    }

    if (move.enpassant) {
        handleEnpassant(move.endSquare, move.movingPieceColor);
    }
    if (move.castling) {
        handleCastling(move.startingSquare, move.endSquare, move.movingPieceColor);
    }


    updateBoards();

    if(isSquareAttacked((move.movingPieceColor == WHITE ? BitBoard(whiteBoards[5]).LS1B() : BitBoard(blackBoards[5]).LS1B()), move.movingPieceColor)){
        undoMove();
        return false;
    }

    if(move.capturedPiece != Piece()){
        captures++;
    }

    colorToMove = (move.movingPieceColor == WHITE ? BLACK : WHITE);
    lastMove = move;
    enPassantEnable.clear();
    return true;
}

void ChessBoard::undoMove() {
    restoreBoardState();
}

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
    if (!boardStateStack.empty()) {
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
    }
}

void ChessBoard::updateBoards()
{
    allWhitePieces = whiteBoards[0] | whiteBoards[1] | whiteBoards[2] | whiteBoards[3] | whiteBoards[4] | whiteBoards[5];
    allBlackPieces = blackBoards[0] | blackBoards[1] | blackBoards[2] | blackBoards[3] | blackBoards[4] | blackBoards[5];
    allPieces = allWhitePieces | allBlackPieces;
    intializeAllSquaresAttackedByBlack();
    intializeAllSquaresAttackedByWhite();

}

void ChessBoard::removePiece(int square) {
    unsigned long long mask = 1ULL << square;

    for(auto &board : whiteBoards){
        board &= ~mask;  
    }
    
    for(auto &board : blackBoards){
        board &= ~mask;  
    }
}

void ChessBoard::placePiece(int square, Piece piece) {
    if (piece.color == WHITE) {
        switch (piece.type) {
            case PAWN:
                whiteBoards[0] |= (1ULL << square);
                break;
            case ROOK:
                whiteBoards[1] |= (1ULL << square);
                break;
            case KNIGHT:
                whiteBoards[2] |= (1ULL << square);
                break;
            case BISHOP:
                whiteBoards[3] |= (1ULL << square);
                break;
            case QUEEN:
                whiteBoards[4] |= (1ULL << square);
                break;
            case KING:
                whiteBoards[5] |= (1ULL << square);
                break;
            default:
                break;
        }
    } else {
        switch (piece.type) {
            case PAWN:
                blackBoards[0] |= (1ULL << square);
                break;
            case ROOK:
                blackBoards[1] |= (1ULL << square);
                break;
            case KNIGHT:
                blackBoards[2] |= (1ULL << square);
                break;
            case BISHOP:
                blackBoards[3] |= (1ULL << square);
                break;
            case QUEEN:
                blackBoards[4] |= (1ULL << square);
                break;
            case KING:
                blackBoards[5] |= (1ULL << square);
                break;
            default:
                break;
        }
    }
}

void ChessBoard::handleEnpassant(int endSquare, Color color) {
    int capturedPawnSquare;
    if (color == WHITE) {
        capturedPawnSquare = endSquare - 8;
    } else {
        capturedPawnSquare = endSquare + 8;
    }
    removePiece(capturedPawnSquare);
}

void ChessBoard::handleCastling(int startSquare, int endSquare, Color color) {
    if (startSquare == E1 && endSquare == G1) { // White kingside castling
        removePiece(H1);
        placePiece(F1, Piece(WHITE, ROOK));
    } 
    else if (startSquare == E1 && endSquare == C1) { // White queenside castling
        removePiece(A1);
        placePiece(D1, Piece(WHITE, ROOK));

    } 
    else if (startSquare == E8 && endSquare == G8) { // Black kingside castling
        removePiece(H8);
        placePiece(F8, Piece(BLACK, ROOK));
    } 
    else if (startSquare == E8 && endSquare == C8) { // Black queenside castling
        removePiece(A8);
        placePiece(D8, Piece(BLACK, ROOK));
    }
}

void ChessBoard::handlePromotion(int endSquare, Color color, PieceType t) {
    removePiece(endSquare);
    placePiece(endSquare, Piece(color, t));
}

void ChessBoard::perft(int depth) {
    if (depth == 0) {
        nodes++;
        return;
    }

    std::vector<Move> moveList;
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
#include <conio.h> 

void ChessBoard::perftTest(int depth, int intialDepth)
{
    if (depth == 0) {
        nodes++;
        return;
    }

    vector<Move> moveList;
    generateMoves(colorToMove, moveList);

    for (auto& move : moveList) {
        saveBoardState();
        if (!makeAMove(move)) {
            continue;
        }
        perftTest(depth - 1, intialDepth);
        undoMove();
    }
}

void ChessBoard::setBoardFromFEN(const std::string& fen) {
    std::istringstream iss(fen);
    std::string boardPart, activeColor, castlingAvailability, enPassantTarget, halfmoveClock, fullmoveNumber;
    iss >> boardPart >> activeColor >> castlingAvailability >> enPassantTarget >> halfmoveClock >> fullmoveNumber;

    whiteBoards = std::vector<unsigned long long>(6, 0ULL);
    blackBoards = std::vector<unsigned long long>(6, 0ULL);
    allWhitePieces = 0ULL;
    allBlackPieces = 0ULL;
    allPieces = 0ULL;

    int square = 63;
    for (char c : boardPart) {
        if (c == '/') {
            continue;
        }
        else if (isdigit(c)) {
            square -= (c - '0');
        }
        else {
            Color color = isupper(c) ? WHITE : BLACK;
            PieceType pieceType;
            switch (tolower(c)) {
                case 'p': pieceType = PAWN; break;
                case 'r': pieceType = ROOK; break;
                case 'n': pieceType = KNIGHT; break;
                case 'b': pieceType = BISHOP; break;
                case 'q': pieceType = QUEEN; break;
                case 'k': pieceType = KING; break;
                default: pieceType = EMPTY; break;
            }

            Piece piece(color, pieceType);
            placePiece(square, piece);
            square--;
        }
    }

    colorToMove = (activeColor == "w") ? WHITE : BLACK;

    canWhiteCastleKingSide = (castlingAvailability.find('K') != std::string::npos);
    canWhiteCastleQueenSide = (castlingAvailability.find('Q') != std::string::npos);
    canBlackCastleKingSide = (castlingAvailability.find('k') != std::string::npos);
    canBlackCastleQueenSide = (castlingAvailability.find('q') != std::string::npos);

    if (enPassantTarget != "-") {
        int file = enPassantTarget[0] - 'a';
        enPassantFile = file;
    } else {
        enPassantFile = -1;
    }

    updateBoards();
    saveBoardState();
}


void ChessBoard::clearBitForAllPieces(int square, Color color)
{
    for(auto board: (color == WHITE ? whiteBoards: blackBoards)){
        board ^= (1ULL << square);
    }
}

void ChessBoard::printChessBoard() {
    Piece currentPiece;
    for (int row = 7; row >= 0; row--) {
        for (int col = 7; col >= 0; col--) {
            int square = row * 8 + col;
            bool pieceFound = false;

            for (int piece = PieceType::FIRST1; piece <= PieceType::LAST1; piece++) {
                currentPiece = Piece(WHITE, static_cast<PieceType>(piece));
                auto currentBoard = BitBoard(getPieceBoard(currentPiece));

                if (currentBoard.getBit(square)) {
                    cout << pieceToChar(currentPiece) << "  ";
                    pieceFound = true;
                    break;
                }

                currentPiece = Piece(BLACK, static_cast<PieceType>(piece));
                currentBoard = getPieceBoard(currentPiece);

                if (currentBoard.getBit(square)) {
                    cout << pieceToChar(currentPiece) << "  ";
                    pieceFound = true;
                    break;
                }
            }

            if (!pieceFound) {
                cout << ".  ";
            }
        }
        cout << "\n";
    }
    cout<<"\n";
    cout << "A  B  C  D  E  F  G  H\n";
    cout<<"\n";

}

string ChessBoard::indexToNotation(int index)
{
    std::string notation;
    notation += 'a' + 7 - (index % 8); 
    notation += '1' + (index / 8); 
    return notation;
}
void ChessBoard::printMove(int startSquare, int endSquare)
{
    std::string startNotation = indexToNotation(startSquare);
    std::string endNotation = indexToNotation(endSquare);
    std::cout << startNotation << endNotation;
}

char ChessBoard::pieceToChar(Piece piece)
{
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

// void ChessBoard::exploreA2A4Branch(int depth) {
//     if (depth == 0) {
//         nodes++;
//         return;
//     }

//     std::vector<Move> moveList;
//     generateMoves(colorToMove, moveList);

//     for (auto& move : moveList) {
//         if (depth == 4 && !(move.startingSquare == 10 && move.endSquare == 26)) {
//             continue; 
//         }

//         saveBoardState();
//         if (makeAMove(move)) {
//             printMove(move.startingSquare, move.endSquare);
//             std::cout << "  nodes: " << nodes << std::endl;
//             printChessBoard();
            
//             while (true) {
//                 char ch = _getch();
//                 if (ch == ' ') {
//                     break;
//                 }
//             }

//             exploreA2A4Branch(depth - 1);
//             undoMove();
//         }
//         restoreBoardState();
//     }
// }

void ChessBoard::initializeHashKeys() {
    for (int color = 0; color < 2; ++color) {
        for (int piece = 0; piece < 6; ++piece) {
            for (int square = 0; square < 64; ++square) {
                zorbristTable[color][piece][square] = randomUInt64();
            }
        }
    }

    turnKey = randomUInt64();

    for (int i = 0; i < 4; ++i) {
        castlingKeys[i] = randomUInt64();
    }

    for (int i = 0; i < 8; ++i) {
        enPassantKeys[i] = randomUInt64();
    }
}
unsigned long long ChessBoard::generateInitialHash() const {
    unsigned long long hash = 0;

    for (int square = 0; square < 64; ++square) {
        auto piece = getPiece(square);
        if (piece.type != EMPTY) {
            hash ^= zorbristTable[piece.color][piece.type][square];
        }
    }

    if (colorToMove == WHITE) {
        hash ^= turnKey;
    }

    if (canWhiteCastleKingSide) {
        hash ^= castlingKeys[0];
    }
    if (canWhiteCastleQueenSide) {
        hash ^= castlingKeys[1];
    }
    if (canBlackCastleKingSide) {
        hash ^= castlingKeys[2];
    }
    if (canBlackCastleQueenSide) {
        hash ^= castlingKeys[3];
    }

    if (enPassantFile >= 0) {
        hash ^= enPassantKeys[enPassantFile];
    }

    return hash;
}

unsigned long long ChessBoard::updateHash(unsigned long long hash, const Move& move) const {
    hash ^= zorbristTable[move.movingPieceColor][move.movingPieceType][move.startingSquare];

    if (move.capturedPiece.type != EMPTY) {
        if(move.enpassant){
            hash ^= zorbristTable[move.capturedPiece.color][move.capturedPiece.type][lastMove.endSquare];
        }
        else{
        hash ^= zorbristTable[move.capturedPiece.color][move.capturedPiece.type][move.endSquare];
        }
    }

    if (move.isPromotion) {
        hash ^= zorbristTable[move.movingPieceColor][move.promotedPieceType][move.endSquare];
    } 
    else {
        hash ^= zorbristTable[move.movingPieceColor][move.movingPieceType][move.endSquare];
    }

    // Handle castling
    if (move.castling) {
        int rookStartingSquare = -1, rookEndingSquare = -1;
        if (move.movingPieceColor == WHITE) {
            if (move.endSquare == 1) {
                rookStartingSquare = 0;
                rookEndingSquare = 2;
            } else if (move.endSquare == 5) {
                rookStartingSquare = 7;
                rookEndingSquare = 4;
            }
        } 
        else {
            if (move.endSquare == 57) {
                rookStartingSquare = 56;
                rookEndingSquare = 58;
            } 
            else if (move.endSquare == 61) {
                rookStartingSquare = 63;
                rookEndingSquare = 60;
            }
        }

        if (rookStartingSquare >= 0 && rookStartingSquare <= 63 && rookEndingSquare >= 0 && rookEndingSquare <= 63) {
            hash ^= zorbristTable[move.movingPieceColor][ROOK][rookStartingSquare];
            hash ^= zorbristTable[move.movingPieceColor][ROOK][rookEndingSquare];
        }
    }

    if (move.movingPieceType == KING) {
        if (move.movingPieceColor == WHITE) {
            if (canWhiteCastleKingSide) {
                hash ^= castlingKeys[0];
            }
            if (canWhiteCastleQueenSide) {
                hash ^= castlingKeys[1];
            }
        } 
        else {
            if (canBlackCastleKingSide) {
                hash ^= castlingKeys[2];
            }
            if (canBlackCastleQueenSide) {
                hash ^= castlingKeys[3];
            }
        }
    }

    if (move.movingPieceType == ROOK) {
        if (move.startingSquare == H1 && move.movingPieceColor == WHITE && canWhiteCastleKingSide) {
            hash ^= castlingKeys[0];
        } 
        else if (move.startingSquare == A1 && move.movingPieceColor == WHITE && canWhiteCastleQueenSide) {
            hash ^= castlingKeys[1];
        } 
        else if (move.startingSquare == H8 && move.movingPieceColor == BLACK && canBlackCastleKingSide) {
            hash ^= castlingKeys[2];
        } 
        else if (move.startingSquare == A8 && move.movingPieceColor == BLACK && canBlackCastleQueenSide) {
            hash ^= castlingKeys[3];
        }
    }

    if(move.capturedPiece.type == ROOK){
            if (move.endSquare == H1 && move.movingPieceColor == BLACK && canWhiteCastleKingSide) {
            hash ^= castlingKeys[0];
        } 
        else if (move.endSquare == A1 && move.movingPieceColor == BLACK && canWhiteCastleQueenSide) {
            hash ^= castlingKeys[1];
        } 
        else if (move.endSquare == H8 && move.movingPieceColor == WHITE && canBlackCastleKingSide) {
            hash ^= castlingKeys[2];
        } 
        else if (move.endSquare == A8 && move.movingPieceColor == WHITE && canBlackCastleQueenSide) {
            hash ^= castlingKeys[3];
        }
    }

    if (enPassantFile >= 0) {
        hash ^= enPassantKeys[enPassantFile];
    }

    hash ^= turnKey;

    return hash;
}
