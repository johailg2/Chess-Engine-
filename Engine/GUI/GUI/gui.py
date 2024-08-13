import sys
import os
import pygame
from enum import Enum

sys.path.append(os.path.dirname(__file__))

import chessBoardWrap  # type: ignore

pygame.init()

window = pygame.display.set_mode((500, 500))
pygame.display.set_caption("Chess GUI")

boardSurface = pygame.Surface((480, 480))
boardSurface.fill((255, 206, 158))

for x in range(0, 8, 2):
    for y in range(0, 8, 2):
        pygame.draw.rect(boardSurface, (210, 180, 140), (x * 60, y * 60, 60, 60))
        pygame.draw.rect(boardSurface, (210, 180, 140), ((x + 1) * 60, (y + 1) * 60, 60, 60))

chessBoard = chessBoardWrap.ChessBoard()
Color = chessBoardWrap.Color
PieceType = chessBoardWrap.PieceType
Piece = chessBoardWrap.Piece
BitBoard = chessBoardWrap.BitBoard
Evaluation = chessBoardWrap.Evaluation()

chessBoard.setFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
chessBoard.initialize()

pieceImages = {
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

def drawBoard(flipped=False):
    window.fill((0, 0, 0))
    window.blit(boardSurface, (10, 10))

    for pieceType in [PieceType.PAWN, PieceType.KNIGHT, PieceType.BISHOP, PieceType.ROOK, PieceType.QUEEN, PieceType.KING]:
        for color in [Color.WHITE, Color.BLACK]:
            currentPiece = Piece(color, pieceType)
            pieceBoard = chessBoard.getPieceBoard(currentPiece)
            while pieceBoard.getBoard():
                square = pieceBoard.LS1B()
                pieceBoard.flipBit(square)
                row = square // 8
                col = square % 8
                pieceImage = pieceImages[(color, pieceType)]
                if flipped:
                    window.blit(pieceImage, ((7 - col) * 60 + 10, (7 - row) * 60 + 10))
                else:
                    window.blit(pieceImage, (col * 60 + 10, row * 60 + 10))

    pygame.display.flip()

def makeEngineMove(color):
    result = Evaluation.search(chessBoard, 6, color == Color.WHITE)
    chessBoard.makeAMove(result.bestMove)
    return True

def makePlayerMove(color, flipped=False):
    selectedSquare = None
    moveList = chessBoard.generateMoves(color)

    while True:
        drawBoard(flipped)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                pos = pygame.mouse.get_pos()
                col = (pos[0] - 10) // 60
                row = (pos[1] - 10) // 60
                if flipped:
                    col = 7 - col
                    row = 7 - row
                square = row * 8 + col

                if selectedSquare is None:
                    piece = chessBoard.getPiece(square)
                    if piece is not None and piece.color == color:
                        selectedSquare = square
                else:
                    move = chessBoardWrap.Move()
                    move.startingSquare = selectedSquare
                    move.endSquare = square
                    for m in moveList:
                        if m.startingSquare == move.startingSquare and m.endSquare == move.endSquare:
                            chessBoard.makeAMove(m)
                            return True
                    selectedSquare = None

def pickColor():
    font = pygame.font.Font(None, 36)
    whiteText = font.render('Play as White', True, (255, 255, 255))
    blackText = font.render('Play as Black', True, (0, 0, 0))
    whiteButton = pygame.Rect(50, 200, 150, 50)
    blackButton = pygame.Rect(300, 200, 150, 50)
    pygame.draw.rect(window, (100, 100, 100), whiteButton)
    pygame.draw.rect(window, (100, 100, 100), blackButton)
    window.blit(whiteText, (60, 210))
    window.blit(blackText, (310, 210))
    pygame.display.flip()

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                pos = pygame.mouse.get_pos()
                if whiteButton.collidepoint(pos):
                    return Color.WHITE
                elif blackButton.collidepoint(pos):
                    return Color.BLACK

running = True
playerColor = pickColor()
isWhiteTurn = playerColor == Color.WHITE
flippedBoard = playerColor == Color.WHITE

while running:
    drawBoard(flippedBoard)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    if isWhiteTurn:
        if playerColor == Color.WHITE:
            if makePlayerMove(Color.WHITE, flippedBoard):
                isWhiteTurn = False
        else:
            if makeEngineMove(Color.WHITE):
                isWhiteTurn = False
    else:
        if playerColor == Color.BLACK:
            if makePlayerMove(Color.BLACK, flippedBoard):
                isWhiteTurn = True
        else:
            if makeEngineMove(Color.BLACK):
                isWhiteTurn = True

    pygame.display.flip()

pygame.quit()
