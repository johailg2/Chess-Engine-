#include "chessboard.h"
#include "bitboards.h"
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdio.h>
#include "AttackTables.h"


using namespace std;

int main() {

  // string s;
  // BitBoard board(1024ULL);
  // board.printBitBoard();
  ChessBoard chess;
  chess.printChessBoard();
  chess.initializeAllSlidingMasks();
  chess.initializeAttackTables(1);
  chess.initializeAttackTables(0);
  chess.initializeAllAttackBoards();
  chess.intializeAllSquaresAttackedByWhite();
  chess.intializeAllSquaresAttackedByBlack();
  BitBoard(chess.getAllSquaresAttackedByBlack()).printBitBoard();
  cout<<chess.isSquareAttacked(A8, BLACK);

;
  // auto b = chess.bishopAttackMasks(G1);
  // BitBoard(b).printBitBoard();
  // BitBoard(1).printBitBoard();
  // for (int i = 0; i < 63; i++){
  //  cout<<chess.findMagicNumber(i,RBits[i], 0)<<",";
  //  cout<<"\n";
  // }
}
