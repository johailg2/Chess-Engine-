#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <iostream>
#include <utility>
#include "bitboards.h"

static const unsigned long long NOT_H_FILE = 0xfefefefefefefefe;
static const unsigned long long NOT_GH_FILE = NOT_H_FILE & (NOT_H_FILE << 1);
static const unsigned long long NOT_A_FILE = 0x7f7f7f7f7f7f7f7f;
static const unsigned long long NOT_AB_FILE = NOT_A_FILE & (NOT_A_FILE >> 1);
static const unsigned long long NOT_TOP_ROW = 0x00ffffffffffffff;
static const unsigned long long NOT_BOTTOM_ROW = 0xffffffffffffff00;


enum ChessSquare {
    H1, G1, F1, E1, D1, C1, B1, A1,
    H2, G2, F2, E2, D2, C2, B2, A2,
    H3, G3, F3, E3, D3, C3, B3, A3,
    H4, G4, F4, E4, D4, C4, B4, A4,
    H5, G5, F5, E5, D5, C5, B5, A5,
    H6, G6, F6, E6, D6, C6, B6, A6,
    H7, G7, F7, E7, D7, C7, B7, A7,
    H8, G8, F8, E8, D8, C8, B8, A8,
    FIRST = H1,
    LAST = A8
};

using namespace std;

enum Color {
    WHITE,
    BLACK
};

enum PieceType {
    EMPTY,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING, 

    FIRST1 = PAWN,
    LAST1 = KING
};

struct Piece {
    Color color;
    PieceType type;

    Piece() : color(WHITE), type(EMPTY) {};

    Piece(Color color_, PieceType type_) : color(color_), type(type_) {};
};

struct Move {
    unsigned int startingSquare : 6; // 6 bits for starting square (0-63)
    unsigned int endSquare : 6;      // 6 bits for end square (0-63)
    unsigned int movingPieceType : 3; // 3 bits for moving piece type (0-7)
    unsigned int movingPieceColor : 1; // 1 bit for moving piece color (0 or 1)
    unsigned int promotedPieceType : 3; // 3 bits for promoted piece type (0-7)
    unsigned int isPromotion : 1; // 1 bit to indicate if promotion is happening
    unsigned int capture : 1; // 1 bit for capture flag
    unsigned int doublePawn : 1; // 1 bit for double pawn move flag
    unsigned int enpassant : 1; // 1 bit for en passant flag
    unsigned int castling : 1; // 1 bit for castling flag

    Move() = default;

    Move(unsigned int ss, unsigned int es, unsigned int mpt, unsigned int mpc, 
    unsigned int ppt, unsigned int prom, unsigned int cap, unsigned int dp, unsigned int enp, unsigned int cast)
        : startingSquare(ss), endSquare(es), movingPieceType(mpt), movingPieceColor(mpc), 
        promotedPieceType(ppt), isPromotion(prom), capture(cap), 
        doublePawn(dp), enpassant(enp), castling(cast) {}

};

class ChessBoard
{
public:

    ChessBoard();

    unsigned long long getPieceBoard(Piece)const;

    inline unsigned long long getWhitePieces()
    {
        return allWhitePieces;
    }

    inline unsigned long long getBlackPieces()
    {
        return allBlackPieces;
    }

    inline unsigned long long getAllPieces(){
        return allPieces;
    }

    inline Color getColor()
    {
        return colorToMove;
    }

    inline unsigned long long getOpposingBoard(Color col){
        return (col == WHITE ? (getBlackPieces()) : (getWhitePieces()));
    }

    inline unsigned long long getFriendlyBoard(Color col) {
        return (col == WHITE ? getWhitePieces() : getBlackPieces());
    }

    void genericMovePopulator(BitBoard attackBoard, int startSquare, Piece piece, int capture);
    void specialMovePopulator(BitBoard moveBoard, int startSquare, Piece piece, int en, int cast, int promotion);

    unsigned long long pawnAttacks(int square, Color color);

    void populatePawnCaptures(Color color);
    void populateQuietPawnMoves(Color color);

    unsigned long long generateKnightAttacks(int square);
    void populateKnightMoves(Color color);

    unsigned long long generateKingAttacks(int square);
    void populateKingMoves(Color color);

    unsigned long long bishopAttackMasks(int square);
    unsigned long long bishopRestrictedAttackMasks(int square, unsigned long long restOfBoard);
    void populateBishopMoves(Color color);

    unsigned long long rookAttackMasks(int square);
    unsigned long long rookRestrictedAttacksMasks(int square, unsigned long long restOfBoard);

    unsigned long long queenAttackMasks(int square);
    unsigned long long queenRestrictedAttackMasks(int square, unsigned long long restOfBoard);

    unsigned long long findMagicNumber(int square, int m, bool bishop);

    void initializeAllSlidingMasks();
    void initializeAttackTables(int bishop);
    void initializeAllAttackBoards();

    unsigned long long getPieceAttackBoard(Piece);
    unsigned long long getAllSquaresAttackedByWhite();
    unsigned long long getAllSquaresAttackedByBlack();

    unsigned long long instantBishopAttacks(unsigned long long occupancy, int square);
    unsigned long long instantRookAttacks(unsigned long long occupancy, int square);
    unsigned long long instantQueenAttacks(unsigned long long occupancy, int square);

    void intializeAllSquaresAttackedByWhite();
    void intializeAllSquaresAttackedByBlack();

    void updateAllSquaresAttackedByWhite(Move move);
    void updateAllSquaresAttackedByBlack(Move move);

    bool isSquareAttacked(int square, Color color);

    void generateMoves(Color color);   

    void printChessBoard();
    char pieceToChar(Piece);

private:
    unsigned long long whitePawns;
    unsigned long long whiteRooks;
    unsigned long long whiteKnights;
    unsigned long long whiteBishops;
    unsigned long long whiteQueen;
    unsigned long long whiteKing;

    /* The black piece positions */
    unsigned long long blackPawns;
    unsigned long long blackRooks;
    unsigned long long blackKnights;
    unsigned long long blackBishops;
    unsigned long long blackQueen;
    unsigned long long blackKing;

    /* Collective positions */
    unsigned long long allWhitePieces;
    unsigned long long allBlackPieces;
    unsigned long long allPieces;

    Color colorToMove;;

    unsigned long long allSquaresAttackedByWhite;
    unsigned long long allSquaresAttackedByBlack;

    vector<Move> moveStorage;
};

unsigned long long attackMaskToOccupancy(int indexOfOccupancyConfig,int numOfSetBits, BitBoard attackMask);

#endif // CHESSBOARD_H
