#include "chessboard.h"
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <random>
#include "AttackTables.h"

    ChessBoard::ChessBoard() {
    // White pieces positions
    whitePawns = 0b0000000000000000000000000000000000000000000000001111111100000000ULL;
    whiteRooks = 0b0000000000000000000000000000000000000000000000000000000010000001ULL;
    whiteKnights = 0b0000000000000000000000000000000000000000000000000000000001000010ULL;
    whiteBishops = 0b0000000000000000000000000000000000000000000000000000000000100100ULL;
    whiteQueen = 0b0000000000000000000000000000000000000000000000000000000000001000ULL; // d1
    whiteKing = 0b0000000000000000000000000000000000000000000000000000000000010000ULL;  // e1

    // Black pieces positions
    blackPawns = 0b0000000011111111000000000000000000000000000000000000000000000000ULL;
    blackRooks = 0b1000000100000000000000000000000000000000000000000000000000000000ULL;
    blackKnights = 0b0100001000000000000000000000000000000000000000000000000000000000ULL;
    blackBishops = 0b0010010000000000000000000000000000000000000000000000000000000000ULL;
    blackQueen = 0b0000100000000000000000000000000000000000000000000000000000000000ULL; // d8
    blackKing = 0b0001000000000000000000000000000000000000000000000000000000000000ULL;  // e8

    // Collective positions
    allWhitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueen | whiteKing;
    allBlackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueen | blackKing;
    allPieces = allWhitePieces | allBlackPieces;

    colorToMove = WHITE;
}

unsigned long long ChessBoard::getPieceBoard(Piece piece) const
{
    switch (piece.type)
    {
    case PAWN:
        if(piece.color == WHITE){
            return whitePawns;
        }
        return blackPawns;

    case ROOK:
        if(piece.color == WHITE){
            return whiteRooks;
        }
        return blackRooks;    

    case BISHOP:
        if(piece.color == WHITE){
            return whiteBishops;
        }
        return blackBishops;  

    case KNIGHT:
        if(piece.color == WHITE){
            return whiteKnights;
        }
        return blackKnights;  

    case QUEEN:
        if(piece.color == WHITE){
            return whiteQueen;
        }
        return blackQueen;   

    case KING:
        if(piece.color == WHITE){
            return whiteKing;
        }
        return blackKing;
    }
    return 0ULL;
}

void ChessBoard::genericMovePopulator(BitBoard attackBoard, int startSquare, Piece piece, int capture)
{
    while (attackBoard.getBoard()) {
        int endSquare = attackBoard.LS1B();
        attackBoard.flipBit(endSquare);
        moveStorage.emplace_back(Move(startSquare, endSquare, piece.type, piece.color, 0, 0, capture, 0, 0, 0));
    }
}

void ChessBoard::specialMovePopulator(BitBoard moveBoard, int startSquare, Piece piece, int en, int cast, int promotion)
{
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

void ChessBoard::populatePawnCaptures(Color color)
{
    BitBoard pawnBoard;
    if(color == WHITE){
        pawnBoard = BitBoard(whitePawns);
    }

    else{
        pawnBoard = BitBoard(blackPawns);
    }

    while(pawnBoard.getBoard()){
        int currentSquare = pawnBoard.LS1B();
        pawnBoard.flipBit(currentSquare);
        BitBoard attackBoard = BitBoard(pawnAttacks(currentSquare, color));
        attackBoard.setBoard(attackBoard.getBoard() & getOpposingBoard(color));
        genericMovePopulator(attackBoard, currentSquare, Piece(color, PAWN), 1);
    }
}

void ChessBoard::populateQuietPawnMoves(Color color)
{
    BitBoard pawnBoard;
    unsigned long long doublePush;
    unsigned long long singlePush;

    if(color == WHITE){
        pawnBoard = BitBoard(whitePawns);
    }
    else{
        pawnBoard = BitBoard(blackPawns);
    }

    while(pawnBoard.getBoard()){
        int currentSquare = pawnBoard.LS1B();
        if(int (currentSquare / 8) == 1 && color == WHITE){
            doublePush = 1 << (currentSquare + 16);
            if(!(doublePush & allPieces)){
                genericMovePopulator(BitBoard(doublePush), currentSquare, Piece(color, PAWN), 0);
            }
        }
        else if(int (currentSquare / 8) == 6 && color == BLACK){
            doublePush = 1 << (currentSquare - 16);
            if(!(doublePush & allPieces)){
                genericMovePopulator(BitBoard(doublePush), currentSquare, Piece(color, PAWN), 0);
            }
        }
        if(color == WHITE){
            singlePush = 1 << (currentSquare + 8);
            if(!(singlePush & allPieces)){
                genericMovePopulator(BitBoard(singlePush), currentSquare, Piece(color, PAWN), 0);
            }
        }
        else if(color == BLACK){
            singlePush = 1 << (currentSquare - 8);
            if(!(singlePush & allPieces)){
                genericMovePopulator(BitBoard(singlePush), currentSquare, Piece(color, PAWN), 0);
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

void ChessBoard::populateKnightMoves(Color color)
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
        if(captureBoard.getBoard()){
            genericMovePopulator(captureBoard, currentSquare, Piece(color, KNIGHT), 1);
        }
        quietBoard = BitBoard(attackBoard ^ friendlyBoard);
        if(quietBoard.getBoard()){
            genericMovePopulator(quietBoard, currentSquare, Piece(color,KNIGHT), 1);
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
    for(int piece = FIRST1; piece <= LAST1; piece++){
        auto currentPiece = Piece(WHITE, static_cast<PieceType>(piece));
        allSquaresAttackedByWhite |= getPieceAttackBoard(currentPiece);
    }
}

void ChessBoard::intializeAllSquaresAttackedByBlack()
{
    allSquaresAttackedByBlack = 0ULL;
    for(int piece = FIRST1; piece <= LAST1; piece++){
        auto currentPiece = Piece(BLACK, static_cast<PieceType>(piece));
        allSquaresAttackedByBlack |= getPieceAttackBoard(currentPiece);
    }
}

bool ChessBoard::isSquareAttacked(int square, Color color)
{
    BitBoard board = 0ULL;
    board.setBit(square);
    if(color == BLACK){
        return (board.getBoard() & allSquaresAttackedByBlack);
    } 
    return board.getBoard() & allSquaresAttackedByWhite;
}

void ChessBoard::generateMoves(Color color)
{
    int startSquare;
    int endSquare;
    BitBoard moveBoard;
    BitBoard opposingBoard;
    BitBoard friendlyBoard;
    Move currentMove;

    if(color == WHITE){
        opposingBoard = BitBoard(getBlackPieces());
        friendlyBoard = BitBoard(getWhitePieces());
    }
    else{
        opposingBoard = BitBoard(getWhitePieces());
        friendlyBoard = BitBoard(getBlackPieces());
    }

    for(int piece = FIRST1; piece <= LAST1; piece++){
        auto currentPiece = Piece(color, static_cast<PieceType>(piece));
        auto currentBoard = BitBoard(getPieceBoard(currentPiece));

        switch (piece)
        {
        case PAWN:
            while(currentBoard.getBoard()){
                int currentSquare = currentBoard.LS1B();
                currentBoard.flipBit(currentSquare);
                moveBoard = BitBoard(pawnAttacks(currentSquare, color));
                moveBoard.setBoard(moveBoard.getBoard() & opposingBoard.getBoard());  
                while(moveBoard.getBoard()){ //handles all captures at a particular square
                    currentMove.startingSquare = currentSquare;
                    currentMove.endSquare = moveBoard.LS1B();
                    moveBoard.flipBit(currentMove.endSquare);
                    currentMove.capture = 1;
                    currentMove.movingPieceColor = color;
                    currentMove.movingPieceType = PAWN;
                    moveStorage.push_back(currentMove);
                    currentMove = Move();
                }

            }
            break;
        
        case KNIGHT:
            /* code */
            break;
        
        case BISHOP:
            /* code */
            break;
        
        case ROOK:
            /* code */
            break;
        
        case QUEEN:
            /* code */
            break;
        
        case KING:
            /* code */
            break;
        
        default:
            break;
        }
    }

}

void ChessBoard::printChessBoard() {
    Piece currentPiece;
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col < 8; col++) {
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

char ChessBoard::pieceToChar(Piece piece) {
    switch (piece.type) {
        case PAWN:   return (piece.color == WHITE) ? 'P' : 'p';
        case KNIGHT: return (piece.color == WHITE) ? 'N' : 'n';
        case BISHOP: return (piece.color == WHITE) ? 'B' : 'b';
        case ROOK:   return (piece.color == WHITE) ? 'R' : 'r';
        case QUEEN:  return (piece.color == WHITE) ? 'Q' : 'q';
        case KING:   return (piece.color == WHITE) ? 'K' : 'k';
        default:     return '.';
    }
}

