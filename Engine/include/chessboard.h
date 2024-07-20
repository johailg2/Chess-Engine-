#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <iostream>
#include <utility>
#include <stack>
#include "bitboards.h"
#include "attackTables.h"

static const unsigned long long NOT_H_FILE = 0xfefefefefefefefe;
static const unsigned long long NOT_GH_FILE = NOT_H_FILE & (NOT_H_FILE << 1);
static const unsigned long long NOT_A_FILE = 0x7f7f7f7f7f7f7f7f;
static const unsigned long long NOT_AB_FILE = NOT_A_FILE & (NOT_A_FILE >> 1);
static const unsigned long long NOT_TOP_ROW = 0x00ffffffffffffff;
static const unsigned long long NOT_BOTTOM_ROW = 0xffffffffffffff00;
static const unsigned long long ROW_5 = 0x00000000ff000000;
static const unsigned long long ROW_4 = 0x000000ff00000000;

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
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING, 
    EMPTY,

    FIRST1 = PAWN,
    LAST1 = KING
};

struct Piece {
    Color color;
    PieceType type;

    Piece() : color(WHITE), type(EMPTY) {};

    Piece(Color color_, PieceType type_) : color(color_), type(type_) {};

    bool operator!=(const Piece& rhs) {
    return (color != rhs.color) && (type != rhs.type);
}
    bool operator==(const Piece& rhs) {
    return (color == rhs.color) && (type == rhs.type);
}

};

struct Move {
    int startingSquare;
    int endSquare;      
    PieceType movingPieceType; 
    Color movingPieceColor; 
    Piece capturedPiece;
    PieceType promotedPieceType;
    bool isPromotion;
    bool doublePawn;
    bool enpassant;
    bool castling;

    Move() = default;

    Move(int ss, int es, PieceType mpt, Color mpc, Piece cp, 
    PieceType ppt, bool prom, bool dp, bool enp, bool cast)
        : startingSquare(ss), endSquare(es), movingPieceType(mpt), movingPieceColor(mpc), capturedPiece(cp), 
        promotedPieceType(ppt), isPromotion(prom), 
        doublePawn(dp), enpassant(enp), castling(cast) {}

    bool operator==(const Move& other) const {
        return this->startingSquare == other.startingSquare &&
               this->endSquare == other.endSquare &&
               this->movingPieceType == other.movingPieceType &&
               this->movingPieceColor == other.movingPieceColor &&
               this->promotedPieceType == other.promotedPieceType &&
               this->capturedPiece.type == other.capturedPiece.type &&
               this->isPromotion == other.isPromotion &&
               this->doublePawn == other.doublePawn &&
               this->enpassant == other.enpassant &&
               this->castling == other.castling;
    }

};

struct BoardState {
    std::vector<unsigned long long> whiteBoards;
    std::vector<unsigned long long> blackBoards;
    unsigned long long allWhitePieces;
    unsigned long long allBlackPieces;
    unsigned long long allPieces;
    Color colorToMove;
    bool canWhiteCastleKingSide;
    bool canBlackCastleKingSide;
    bool canWhiteCastleQueenSide;
    bool canBlackCastleQueenSide;
    bool isThereCheck;
    unsigned long long allSquaresAttackedByWhite;
    unsigned long long allSquaresAttackedByBlack;
    Move lastMove;
    unsigned long long key;
};


class ChessBoard
{
public:

    ChessBoard();

    void initialize();

    unsigned long long getPieceBoard(Piece) const;
    Piece getPiece(int square) const;

    void setPieceBoard(Piece piece, unsigned long long newBoard);

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

    bool isSquareAttacked(int square, Color color);

    void generateMoves(Color color, vector<Move>&); 
    void generateCaptureMoves(Color color, vector<Move>& captureList);
    bool makeAMove(Move &move);

    void saveBoardState();
    void restoreBoardState();

    void perft(int depth);
    void perftTest(int depth, int intialDepth);

    void undoMove();

    void setBoardFromFEN(const std::string& fen);

    void clearBitForAllPieces(int square, Color color);

    void printChessBoard();
    void removePiece(int square);
    void placePiece(int square, Piece color);
    void printMove(int startSquare, int endSquare);
    void initializeHashKeys();
    unsigned long long generateInitialHash() const;
    unsigned long long updateHash(unsigned long long hash, const Move& move) const;
    unsigned long long nodes = 0;
    unsigned long long currentHash;
    unsigned long long zorbristTable[2][6][64];
    unsigned long long castlingKeys[4];
    unsigned long long enPassantKeys[8];
    unsigned long long turnKey;
    int enPassantFile;
    Move lastMove ;

private:

    vector<unsigned long long> whiteBoards;
    // unsigned long long whitePawns; 0
    // unsigned long long whiteRooks; 1
    // unsigned long long whiteKnights; 2
    // unsigned long long whiteBishops; 3
    // unsigned long long whiteQueen; 4 
    // unsigned long long whiteKing; 5

    vector<unsigned long long> blackBoards;
    // unsigned long long blackPawns;
    // unsigned long long blackRooks;
    // unsigned long long blackKnights;
    // unsigned long long blackBishops;
    // unsigned long long blackQueen;
    // unsigned long long blackKing;

    unsigned long long allWhitePieces;
    unsigned long long allBlackPieces;
    unsigned long long allPieces;

    Color colorToMove;

    bool canWhiteCastleKingSide;
    bool canBlackCastleKingSide;

    bool canWhiteCastleQueenSide;
    bool canBlackCastleQueenSide;

    bool isThereCheck;

    unsigned long long allSquaresAttackedByWhite;
    unsigned long long allSquaresAttackedByBlack;

    Move lastMoveBackup;

    std::stack<BoardState> boardStateStack;

    std::string indexToNotation(int index);
    char pieceToChar(Piece);

    int captures = 0;
    int checks;

    void updateBoards();

    void handleEnpassant(int square, Color color);
    void handleCastling(int square, int esquare, Color color);
    void handlePromotion(int endSquare, Color color, PieceType);
    
    unsigned long long pawnAttacks(int square, Color color);
    void populatePawnCaptures(Color color, Move lastMove, vector<Move>&);
    void populateQuietPawnMoves(Color color, vector<Move>&);

    unsigned long long generateKnightAttacks(int square);
    void populateKnightMoves(Color color, vector<Move>&, bool captureOnly);

    unsigned long long generateKingAttacks(int square);
    void populateKingMoves(Color color, vector<Move>&, bool captureOnly);

    unsigned long long bishopAttackMasks(int square);
    unsigned long long bishopRestrictedAttackMasks(int square, unsigned long long restOfBoard);
    void populateBishopMoves(Color color, vector<Move>&, bool captureOnly);

    unsigned long long rookAttackMasks(int square);
    unsigned long long rookRestrictedAttacksMasks(int square, unsigned long long restOfBoard);
    void populateRookMoves(Color color, vector<Move>&, bool captureOnly);

    unsigned long long queenAttackMasks(int square);
    unsigned long long queenRestrictedAttackMasks(int square, unsigned long long restOfBoard);
    void populateQueenMoves(Color color, vector<Move>&, bool captureOnly);

    unsigned long long findMagicNumber(int square, int m, bool bishop);

    void initializeAllSlidingMasks();
    void initializeAttackTables(int bishop);

    unsigned long long getAllSquaresAttackedByWhite();
    unsigned long long getAllSquaresAttackedByBlack();

    unsigned long long instantBishopAttacks(unsigned long long occupancy, int square);
    unsigned long long instantRookAttacks(unsigned long long occupancy, int square);
    unsigned long long instantQueenAttacks(unsigned long long occupancy, int square);

    void intializeAllSquaresAttackedByWhite();
    void intializeAllSquaresAttackedByBlack();

    vector<Move> enPassantEnable;


};

unsigned long long attackMaskToOccupancy(int indexOfOccupancyConfig,int numOfSetBits, BitBoard attackMask);

#endif // CHESSBOARD_H
