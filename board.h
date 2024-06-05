// #include <array>
// #include <utility>
// #include <algorithm>
// #include <vector>
// #include <tuple>
// #include <iostream>
// #include <random>
// #include <string>
// #include <sstream>


// using namespace std;

// enum PieceType {
//     EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
// };

// enum Color {
//     WHITE, BLACK, NONE
// };

// struct Piece {
//     PieceType type_; 
//     Color color_;

//     Piece() : type_(EMPTY), color_(NONE) {} 

//     Piece(PieceType type, Color color) : type_(type), color_(color) {} 

//     bool operator==(const Piece otherPiece) const{
//         if(type_ == otherPiece.type_ && color_ == otherPiece.color_){
//             return true;
//         }
//         return false;
//     }
// };

// struct Position {
//     int row;
//     int col;
//     Piece piece;

//     Position(): row(-1), col(-1), piece(Piece()){}
//     Position(int r, int c, Piece p) : row(r), col(c), piece(p) {}

//     bool operator!=(const Position otherPosition) const{
//         if(row == otherPosition.row && col == otherPosition.col && piece == otherPosition.piece){
//             return false;
//         }
//         return true;
//     }

// };

// struct Move {
//     Position startPosition;
//     Position endPosition;
//     Move() : startPosition(Position(-1,-1,Piece())), endPosition(Position(-1,-1,Piece())) {}
//     Move(Position start, Position end) : startPosition(start), endPosition(end) {}

//     bool operator==(const Move& otherMove) const{
//         if(startPosition.col == otherMove.startPosition.col && startPosition.row == otherMove.startPosition.row 
//            && endPosition.col == otherMove.endPosition.col && endPosition.row == otherMove.endPosition.row
//            && startPosition.piece.color_ == otherMove.startPosition.piece.color_ && 
//            startPosition.piece.type_ == otherMove.startPosition.piece.type_ &&
//            endPosition.piece.color_ == otherMove.endPosition.piece.color_ &&
//            endPosition.piece.type_ == otherMove.endPosition.piece.type_
//            ){
//             return true;
//            }
//            return false;
//     }
// };

// class Board {
// public:
//     Board();

//     ~Board() = default;

//     Board(const Board& otherBoard) : board_(otherBoard.board_) {}

//     Board& operator=(const Board& otherBoard) {
//         board_ = otherBoard.board_;
//         return *this;
//     }

//     Color whooseMove(){
//         return colorToMove;
//     }

//     Move getLastMove(){
//         return lastMove;
//     }

//     vector<Move> setOfAllPossibleMoves(Board& currentBoard);

//     vector<Move> generatePawnMoves(Position& currentPosition);
//     vector<Move> generateKnightMoves(Position& currentPosition);
//     vector<Move> generateSlidingMoves(Position& currentPosition, const vector<pair<int,int>>& offsets);
//     vector<Move> generateBishopMoves(Position& currentPosition);
//     vector<Move> generateRookMoves(Position& currentPosition);
//     vector<Move> generateQueenMoves(Position& currentPosition);
//     vector<Move> generateKingMoves(Position& currentPosition);

//     Position findAPiece(Board& currentBoard, Piece) const;

//     vector<Position> isThereAnAttack(Board& board, Position attackedPosition) const;

//     pair<bool, Position> checkLine(int kingRow, int kingCol, int dRow, int dCol) const;

//     pair<bool, Position> knightCheck(int kingRow, int kingCol) const;

//     pair<bool, Position> pawnCheck(int kingRow, int kingCol) const;

//     vector<Move> setOfAllLegalMoves(Board& currentBoard);

//     void makeMove(Board& currentBoard, Move move);

//     void undoMove(Board& currentBoard, Move move);

//     Move parseMove(const std::string& input) const;

//     Move selectRandomMove(const std::vector<Move>& moves) const;

//     array<array<Piece, 8>, 8> getBoard(){return board_;};

//     Color getColor(){return colorToMove;};    

// private:
//     array<array<Piece, 8>, 8> board_{{Piece()}};
//     Color colorToMove = WHITE;
//     Move lastMove;
//     int moveCount;
// };
