import sys
import os
import pygame
from enum import Enum
import random

# Add the current directory to the system path
sys.path.append(os.path.dirname(__file__))

import chessBoardWrap  # type: ignore # Import the compiled Python extension

pygame.init()

# Set the window size
window = pygame.display.set_mode((500, 500))
pygame.display.set_caption("Chess GUI")

boardSurface = pygame.Surface((480, 480))
boardSurface.fill((255, 206, 158))

# Draw the board pattern
for x in range(0, 8, 2):
    for y in range(0, 8, 2):
        pygame.draw.rect(boardSurface, (210, 180, 140), (x * 60, y * 60, 60, 60))
        pygame.draw.rect(boardSurface, (210, 180, 140), ((x + 1) * 60, (y + 1) * 60, 60, 60))

chessBoard = chessBoardWrap.ChessBoard()
Color = chessBoardWrap.Color
PieceType = chessBoardWrap.PieceType
Piece = chessBoardWrap.Piece
BitBoard = chessBoardWrap.BitBoard

chessBoard.initializeAllSlidingMasks()
chessBoard.initializeAttackTables(1)
chessBoard.initializeAttackTables(0)

piece_images = {
    (Color.WHITE, PieceType.PAWN): pygame.image.load("Piece Images/w_p.png"),
    (Color.WHITE, PieceType.KNIGHT): pygame.image.load("Piece Images/w_n.png"),
    (Color.WHITE, PieceType.BISHOP): pygame.image.load("Piece Images/w_b.png"),
    (Color.WHITE, PieceType.ROOK): pygame.image.load("Piece Images/w_r.png"),
    (Color.WHITE, PieceType.QUEEN): pygame.image.load("Piece Images/w_q.png"),
    (Color.WHITE, PieceType.KING): pygame.image.load("Piece Images/w_k.png"),
    (Color.BLACK, PieceType.PAWN): pygame.image.load("Piece Images/b_p.png"),
    (Color.BLACK, PieceType.KNIGHT): pygame.image.load("Piece Images/b_n.png"),
    (Color.BLACK, PieceType.BISHOP): pygame.image.load("Piece Images/b_b.png"),
    (Color.BLACK, PieceType.ROOK): pygame.image.load("Piece Images/b_r.png"),
    (Color.BLACK, PieceType.QUEEN): pygame.image.load("Piece Images/b_q.png"),
    (Color.BLACK, PieceType.KING): pygame.image.load("Piece Images/b_k.png"),
}

def drawBoard():
    window.fill((0, 0, 0))  # Clear the window
    window.blit(boardSurface, (10, 10))  # Draw the board surface

    for pieceType in [PieceType.PAWN, PieceType.KNIGHT, PieceType.BISHOP, PieceType.ROOK, PieceType.QUEEN, PieceType.KING]:
        # Check for a white piece
        currentPiece = Piece(Color.WHITE, pieceType)
        pieceBoard = chessBoard.getPieceBoard(currentPiece)
        while pieceBoard.getBoard():
            square = pieceBoard.LS1B()
            pieceBoard.flipBit(square)
            row = square // 8
            col = square % 8
            piece_image = piece_images[(Color.WHITE, pieceType)]
            window.blit(piece_image, (col * 60 + 10, row * 60 + 10))  # Draw the piece on the board

        # Check for a black piece
        currentPiece = Piece(Color.BLACK, pieceType)
        pieceBoard = chessBoard.getPieceBoard(currentPiece)
        while pieceBoard.getBoard():
            square = pieceBoard.LS1B()
            pieceBoard.flipBit(square)
            row = square // 8
            col = square % 8
            piece_image = piece_images[(Color.BLACK, pieceType)]
            window.blit(piece_image, (col * 60 + 10, row * 60 + 10))  # Draw the piece on the board

    pygame.display.flip()  # Update the display

def makeRandomMove(color):
    chessBoard.generateMoves(color)
    move_count = len(chessBoard.moveStorage)
    if move_count > 0:
        randomMove = chessBoard.moveStorage[random.randint(0, move_count - 1)]
        chessBoard.makeAMove(randomMove)
        drawBoard()
        return True
    return False

# Game loop
running = True
is_white_turn = True

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE:
                if is_white_turn:
                    if makeRandomMove(Color.WHITE):
                        is_white_turn = False
                    else:
                        print("No valid moves for White!")
                else:
                    if makeRandomMove(Color.BLACK):
                        is_white_turn = True
                    else:
                        print("No valid moves for Black!")

pygame.quit()
