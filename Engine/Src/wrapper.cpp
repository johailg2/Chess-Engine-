// wrapper.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/chessboard.h"
#include "../include/attackTables.h"
#include "../include/bitboards.h"
#include "../include/eval.h"

namespace py = pybind11;

PYBIND11_MODULE(chessBoardWrap, m) {
    py::enum_<Color>(m, "Color")
        .value("WHITE", WHITE)
        .value("BLACK", BLACK)
        .export_values();

    py::enum_<PieceType>(m, "PieceType")
        .value("EMPTY", EMPTY)
        .value("PAWN", PAWN)
        .value("KNIGHT", KNIGHT)
        .value("BISHOP", BISHOP)
        .value("ROOK", ROOK)
        .value("QUEEN", QUEEN)
        .value("KING", KING)
        .export_values();

    py::class_<Piece>(m, "Piece")
        .def(py::init<>())
        .def(py::init<Color, PieceType>())
        .def_readwrite("color", &Piece::color)
        .def_readwrite("type", &Piece::type);

    py::class_<Move>(m, "Move")
        .def(py::init<>()) 
        .def(py::init<unsigned int, unsigned int, PieceType, Color, Piece, PieceType, bool, bool, bool, bool>())
        .def_readwrite("startingSquare", &Move::startingSquare)
        .def_readwrite("endSquare", &Move::endSquare)
        .def_readwrite("movingPieceType", &Move::movingPieceType)
        .def_readwrite("movingPieceColor", &Move::movingPieceColor)
        .def_readwrite("capturePiece", &Move::capturedPiece)
        .def_readwrite("promotedPieceType", &Move::promotedPieceType)
        .def_readwrite("isPromotion", &Move::isPromotion)
        .def_readwrite("doublePawn", &Move::doublePawn)
        .def_readwrite("enpassant", &Move::enpassant)
        .def_readwrite("castling", &Move::castling);

    py::class_<BitBoard>(m, "BitBoard")
        .def(py::init<>())
        .def("getBit", &BitBoard::getBit)
        .def("flipBit", &BitBoard::flipBit)
        .def("setBit", &BitBoard::setBit)
        .def("getBoard", &BitBoard::getBoard)
        .def("setBoard", &BitBoard::setBoard)
        .def("LS1B", &BitBoard::LS1B);

    py::class_<Evaluation>(m, "Evaluation")
        .def(py::init<>())
        .def("search", &Evaluation::iterativeDeepeningSearch, py::arg("board"), py::arg("depth"), py::arg("player"));


    py::class_<ChessBoard>(m, "ChessBoard")
        .def(py::init<>())
        .def("initialize", &ChessBoard::initialize)
        .def("getPiece", &ChessBoard::getPiece)
        .def("getPieceBoard", [](const ChessBoard &cb, const Piece &piece) {
            return BitBoard(cb.getPieceBoard(piece));
        })
        .def("generateMoves", [](ChessBoard& board, Color color) {
            std::vector<Move> moveList;
            board.generateMoves(color, moveList);
            return moveList;
        })  // Returns a vector of Move objects to Python        
        .def("makeAMove", &ChessBoard::makeAMove)
        .def("setPieceBoard", &ChessBoard::setPieceBoard)
        .def("getPieceBoard", &ChessBoard::getPieceBoard)
        .def("getWhitePieces", &ChessBoard::getWhitePieces)
        .def("getBlackPieces", &ChessBoard::getBlackPieces)
        .def("getColor", &ChessBoard::getColor)
        .def("getOpposingBoard", &ChessBoard::getOpposingBoard)
        .def("getFriendlyBoard", &ChessBoard::getFriendlyBoard)
        .def("isSquareAttacked", &ChessBoard::isSquareAttacked)
        .def("printChessBoard", &ChessBoard::printChessBoard)
        .def("setFromFen", &ChessBoard::setBoardFromFEN);

    py::class_<SearchResult>(m, "SearchResult")
        .def(py::init<>())
        .def_readwrite("evaluation", &SearchResult::evaluation)
        .def_readwrite("bestMove", &SearchResult::bestMove)
        .def_readwrite("pv", &SearchResult::pv);
}
        
