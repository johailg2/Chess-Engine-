// #include "board.h"
// #include <tuple>

// Board::Board()
// {
//     board_[0][0] = board_[0][7] = Piece(ROOK, WHITE);
//     board_[0][1] = board_[0][6] = Piece(KNIGHT, WHITE);
//     board_[0][2] = board_[0][5] = Piece(BISHOP, WHITE);
//     board_[0][3] = Piece(QUEEN, WHITE);
//     board_[0][4] = Piece(KING, WHITE);

//     for(int i = 0 ; i < 8; i++){
//         board_[1][i] = Piece(PAWN, WHITE);
//         board_[6][i] = Piece(PAWN, BLACK);
//     }

// board_[7][0] = board_[7][7] = Piece(ROOK, BLACK);
// board_[7][1] = board_[7][6] = Piece(KNIGHT, BLACK);
// board_[7][2] = board_[7][5] = Piece(BISHOP, BLACK);
// board_[7][3] = Piece(QUEEN, BLACK);
// board_[7][4] = Piece(KING, BLACK);

// }

// vector<Move> Board::setOfAllPossibleMoves(Board &currentBoard)
// {
//     vector<Move> setOfAllMoves; 
//     for(int row = 0 ; row < 8; row++){
//         for(int col = 0 ; col < 8; col++){
//             auto currentPiece = board_[row][col];
//             auto currentPosition = Position(row, col, currentPiece);
//             if(currentPiece.color_ == colorToMove){
//                 auto pawnMoves = generatePawnMoves(currentPosition);
//                 switch (currentPiece.type_)
//                 {
//                 case PAWN:
//                     setOfAllMoves.insert(setOfAllMoves.end(), pawnMoves.begin(), pawnMoves.end());                    //FIGURE OUT EN PASSANT LATER
//                     // if(colorToMove == WHITE){
//                     //     if(col > 0){
//                     //     if(board_[row][col].type_ == PAWN && board_[row][col].color_ == WHITE && row == 4){
//                     //     Position blackPawnStart = Position(6,col-1,Piece(PAWN,BLACK));
//                     //     Position blackPawnEnd = Position(4,col-1,Piece(PAWN,BLACK));
//                     //     Move leftofPawn(blackPawnStart,blackPawnEnd);
//                     //     if(getLastMove() == leftofPawn){
//                     //         setOfAllMoves.push_back()
//                     //     }
//                     //     }
//                     //     }
//                     // }
//                     break;

//                 case KNIGHT:
//                     setOfAllMoves.insert(setOfAllMoves.end(), generateKnightMoves(currentPosition).begin(), generateKnightMoves(currentPosition).end());
//                     break;

//                 case BISHOP:
//                     setOfAllMoves.insert(setOfAllMoves.end(), generateBishopMoves(currentPosition).begin(), generateBishopMoves(currentPosition).end());
//                     break;

//                 case ROOK:
//                     setOfAllMoves.insert(setOfAllMoves.end(), generateRookMoves(currentPosition).begin(), generateRookMoves(currentPosition).end());
//                     break;

//                 case QUEEN:
//                     setOfAllMoves.insert(setOfAllMoves.end(), generateQueenMoves(currentPosition).begin(), generateQueenMoves(currentPosition).end());
//                     break;
//                 case KING:
//                     setOfAllMoves.insert(setOfAllMoves.end(), generateKingMoves(currentPosition).begin(), generateKingMoves(currentPosition).end());

//                     //add castling
//                     break;
//                 }
//             }
//         }
//     }
// }

// vector<Move> Board::generatePawnMoves(Position &currentPosition)
// {
//     vector<Move> pawnMoves;
//     Piece currentPiece = currentPosition.piece;
//     int row = currentPosition.row;
//     int col = currentPosition.col;
//     if(colorToMove == WHITE){
//         if(board_[row + 1][col-1].color_ == BLACK && col > 0){
//             pawnMoves.push_back(Move(currentPosition, Position(row+1,col-1,currentPiece)));
//         }

//         if(board_[row+1][col+1].color_ == BLACK && col < 7){
//             pawnMoves.push_back(Move(currentPosition, Position(row+1,col+1,currentPiece)));
//         }

//         //single push
//         if(board_[row+1][col].color_ == NONE){
//             pawnMoves.push_back(Move(currentPosition, Position(row+1,col,currentPiece)));
//         }
//         //double push
//         if(row == 1 && board_[row+2][col].color_ == NONE){ 
//             pawnMoves.push_back(Move(currentPosition, Position(row+2,col,currentPiece)));
//         }
//     }
//     else{
//         if(board_[row - 1][col-1].color_ == WHITE && col > 0){
//             pawnMoves.push_back(Move(currentPosition, Position(row-1,col-1,currentPiece)));
//         }

//         if(board_[row-1][col+1].color_ == WHITE && col < 7){
//             pawnMoves.push_back(Move(currentPosition, Position(row-1,col+1,currentPiece)));
//         }

//         //single push
//         if(board_[row-1][col].color_ == NONE){
//             pawnMoves.push_back(Move(currentPosition, Position(row-1,col,currentPiece)));
//         }
//         //double push
//         if(row == 6 && board_[row-2][col].color_ == NONE){ 
//             pawnMoves.push_back(Move(currentPosition, Position(row-2,col,currentPiece)));
//         }
//     }
// }

// vector<Move> Board::generateKnightMoves(Position &currentPosition)
// {
//     vector<Move> knightMoves;
//     int row = currentPosition.row;
//     int col = currentPosition.col;
//     array<pair<int, int>, 8> possibleKnightMoves = {{{2, 1}, {1, 2}, {-1, 2}, {-2, 1},{-2, -1}, {-1, -2}, {1, -2}, {2, -1}}};

//     for (auto move : possibleKnightMoves) {
//         int moveRow = row + move.first;
//         int moveCol = col + move.second;
//         if (moveRow >= 0 && moveRow < 8 && moveCol >= 0 && moveCol < 8) {
//             Color occupiedColor = board_[moveRow][moveCol].color_;
//             if (occupiedColor != colorToMove) { 
//                 knightMoves.push_back(Move(currentPosition, Position(moveRow, moveCol, board_[row][col])));
//             }
//         }
//     }
//     return knightMoves;
// }

// vector<Move> Board::generateSlidingMoves(Position &currentPosition, const vector<pair<int, int>> &directions) {
//     vector<Move> moves;
//     for (auto &[dRow, dCol] : directions) {
//         int row = currentPosition.row;
//         int col = currentPosition.col;
//         while (true) {
//             row += dRow;
//             col += dCol;
//             if (row < 0 || row >= 8 || col < 0 || col >= 8) break;

//             Color occupiedColor = board_[row][col].color_;
//             if (occupiedColor == colorToMove) {
//                 break;
//             }
//             moves.emplace_back(currentPosition, Position(row, col, board_[row][col]));
//             if (occupiedColor != NONE) {
//                 break;
//             }
//         }
//     }
//     return moves;
// }

// vector<Move> Board::generateBishopMoves(Position &currentPosition) {
//     return generateSlidingMoves(currentPosition, {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}});
// }

// vector<Move> Board::generateRookMoves(Position &currentPosition) {
//     return generateSlidingMoves(currentPosition, {{0, 1}, {0, -1}, {1, 0}, {-1, 0}});
// }

// vector<Move> Board::generateQueenMoves(Position &currentPosition) {
//     auto diagonalMoves = generateBishopMoves(currentPosition);
//     auto straightMoves = generateRookMoves(currentPosition);
//     diagonalMoves.insert(diagonalMoves.end(), straightMoves.begin(), straightMoves.end());
//     return diagonalMoves;
// }


// vector<Move> Board::generateKingMoves(Position &currentPosition) {
//     vector<Move> kingMoves;
//     int row = currentPosition.row;
//     int col = currentPosition.col;
//     array<pair<int, int>, 8> possibleKingMoves = {{{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

//     for (auto move : possibleKingMoves) {
//         int moveRow = row + move.first;
//         int moveCol = col + move.second;
//         if (moveRow >= 0 && moveRow < 8 && moveCol >= 0 && moveCol < 8) {
//             Color occupiedColor = board_[moveRow][moveCol].color_;
//             if (occupiedColor != colorToMove) {
//                 kingMoves.push_back(Move(currentPosition, Position(moveRow, moveCol, board_[moveRow][moveCol])));
//             }
//         }
//     }

//     return kingMoves;
// }

// Position Board::findAPiece(Board &currentBoard, Piece pieceToFind) const
// {
//     for(int row = 0; row < 8; row++){
//         for(int col = 0; col < 8; col++){
//             if(board_[row][col] == pieceToFind){
//                 return Position(row, col, pieceToFind);
//             }
//         }
//     }
//     return Position();
// }

// vector<Position> Board::isThereAnAttack(Board &currentBoard, Position attackedPosition) const
// {

//     vector<Position> returnAttacks;
//     int attackedRow = attackedPosition.row;
//     int attackedCol = attackedPosition.col;

//     auto knightAttack = knightCheck(attackedRow, attackedCol);
//     if(knightAttack.first){
//         returnAttacks.push_back(knightAttack.second);
//     }

//     auto pawnAttack = pawnCheck(attackedRow, attackedCol);
//     if(pawnAttack.first){
//         returnAttacks.push_back(pawnAttack.second);
//     }

//     auto bottomRightDiag = checkLine(attackedRow, attackedCol, 1, 1);
//     if(bottomRightDiag.first){
//         returnAttacks.push_back(bottomRightDiag.second);
//     }

//     auto bottomLeftDiag = checkLine(attackedRow, attackedCol, 1, -1);
//     if(bottomLeftDiag.first){
//         returnAttacks.push_back(bottomLeftDiag.second);
//     }

//     auto topRightDiag = checkLine(attackedRow, attackedCol, -1, 1);
//     if(topRightDiag.first){
//         returnAttacks.push_back(topRightDiag.second);
//     }

//     auto topLeftDiag = checkLine(attackedRow, attackedCol, -1, -1);
//     if(topLeftDiag.first){
//         returnAttacks.push_back(topLeftDiag.second);

//     }

//     auto right = checkLine(attackedRow, attackedCol, 0, 1);
//     if(right.first){
//         returnAttacks.push_back(right.second);
//     }

//     auto up = checkLine(attackedRow, attackedCol, 1, 0);
//     if(up.first){
//         returnAttacks.push_back(up.second);
//     }

//     auto left = checkLine(attackedRow, attackedCol, 0, -1); 
//     if(left.first){
//         returnAttacks.push_back(left.second);
//     }

//     auto down = checkLine(attackedRow, attackedCol, -1, 0); 
//     if(down.first){
//         returnAttacks.push_back(down.second);
//     }

//     return returnAttacks;
// }

// pair<bool, Position> Board::checkLine(int kingRow, int kingCol, int dRow, int dCol) const
// {
//     int row = kingRow + dRow;
//     int col = kingCol + dCol;
//     while (row >= 0 && row < 8 && col >= 0 && col < 8) {
//         Piece currentPiece = board_[row][col];
//         if (currentPiece.color_ != NONE) {
//             if (currentPiece.color_ == colorToMove){
//                 break;
//             }
//             if (currentPiece.type_ == QUEEN || currentPiece.type_ == (abs(dRow) == abs(dCol) ? BISHOP : ROOK)) {
//                 Piece checkingPiece = Piece(currentPiece.type_, currentPiece.color_);
//                 return make_pair(true, Position(row, col, checkingPiece));
//             }
//             break;
//         }
//         row += dRow;
//         col += dCol;
//     }
    
//     return make_pair(false, Position());
// }

// pair<bool, Position> Board::knightCheck(int kingRow, int kingCol) const
// {
//     int knightMoves[8][2] = {{-2, -1}, {-1, -2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}, {-1, 2}, {-2, 1}};

//     for (int i = 0; i < 8; i++) {
//         int knightRow = kingRow + knightMoves[i][0];
//         int knightCol = kingCol + knightMoves[i][1];

//         if (knightRow >= 0 && knightRow < 8 && knightCol >= 0 && knightCol < 8) {
//             if (board_[knightRow][knightCol].color_ == NONE || board_[knightRow][knightCol].color_ == colorToMove) {
//                 continue;
//             }
//             else if(board_[knightRow][knightCol].type_ == KNIGHT){
//                 return make_pair(true,Position(knightRow, knightCol, board_[knightRow][knightCol]));
//             }
//         }
//     }
//     return make_pair(false, Position());
// }

// pair<bool, Position> Board::pawnCheck(int kingRow, int kingCol) const
// { 
//     int pawnOffsets[2][2] = {{1, -1}, {1, 1}};
//     if (colorToMove == BLACK) {
//         pawnOffsets[0][0] = -1;
//         pawnOffsets[1][0] = -1;
//     }

//     for (int i = 0; i < 2; i++) {
//         int pawnRow = kingRow + pawnOffsets[i][0];
//         int pawnCol = kingCol + pawnOffsets[i][1];
//         if (pawnRow >= 0 && pawnRow < 8 && pawnCol >= 0 && pawnCol < 8) {
//             Piece pawnPiece = board_[pawnRow][pawnCol];
//             if (pawnPiece.type_ == PAWN && pawnPiece.color_ != colorToMove) {
//                 return make_pair(true, Position(pawnRow, pawnCol, board_[pawnRow][pawnCol]));
//             }
//         }
//     }
//     return make_pair(false, Position());
// }

// vector<Move> Board::setOfAllLegalMoves(Board &currentBoard)
// {
//     auto setOfAllMoves = setOfAllPossibleMoves(currentBoard);

//     for(auto it = setOfAllMoves.begin(); it != setOfAllMoves.end(); ++it){
//         makeMove(currentBoard, *it);
//         if(!isThereAnAttack(currentBoard, findAPiece(currentBoard, Piece(KING, colorToMove))).empty()){
//             setOfAllMoves.erase(it);
//         }
//         undoMove(currentBoard, *it);
//     }
//     return setOfAllMoves;
// }

// void Board::makeMove(Board &currentBoard, Move move)
// {
//     Piece emptyPiece = Piece(EMPTY, NONE);
//     int startRow = move.startPosition.row;
//     int startCol = move.startPosition.col;
//     int endRow = move.endPosition.row;
//     int endCol = move.endPosition.col;
//     Piece piece = move.startPosition.piece;

//     board_[startRow][startCol] = emptyPiece; 

//     board_[endRow][endCol] = piece; 

//     colorToMove = (colorToMove == WHITE) ? BLACK : WHITE; 
//     moveCount++;
// }

// void Board::undoMove(Board &currentBoard, Move move)
// {
//     Piece emptyPiece = Piece(EMPTY, NONE);
//     int startRow = move.startPosition.row;
//     int startCol = move.startPosition.col;
//     int endRow = move.endPosition.row;
//     int endCol = move.endPosition.col;
//     Piece piece = move.endPosition.piece;

//     board_[endRow][endCol] = move.startPosition.piece;
//     board_[startRow][startCol] = piece;

//     colorToMove = (colorToMove == WHITE) ? BLACK : WHITE;  
//     moveCount--;
// }

// Move Board::parseMove(const std::string &input) const
// {
//     int startCol = input[0] - 'a';
//     int startRow = 8 - (input[1] - '0');
//     int endCol = input[2] - 'a';
//     int endRow = 8 - (input[3] - '0');

//     Position start(startRow, startCol, {});
//     Position end(endRow, endCol, {});
//     return Move(start, end);
// }

// Move Board::selectRandomMove(const std::vector<Move> &moves) const
// {
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<> dis(0, moves.size() - 1);
//     return moves[dis(gen)];
// }
