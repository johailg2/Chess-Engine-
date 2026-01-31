import sys
import os
import pygame

current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(current_dir)

import chessBoardWrap # type: ignore

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

img_fldr = os.path.join(current_dir, "Piece Images")
pieceImages = {
    (Color.WHITE, PieceType.PAWN):   pygame.image.load(os.path.join(img_fldr, "w_p.png")),
    (Color.WHITE, PieceType.KNIGHT): pygame.image.load(os.path.join(img_fldr, "w_n.png")),
    (Color.WHITE, PieceType.BISHOP): pygame.image.load(os.path.join(img_fldr, "w_b.png")),
    (Color.WHITE, PieceType.ROOK):   pygame.image.load(os.path.join(img_fldr, "w_r.png")),
    (Color.WHITE, PieceType.QUEEN):  pygame.image.load(os.path.join(img_fldr, "w_q.png")),
    (Color.WHITE, PieceType.KING):   pygame.image.load(os.path.join(img_fldr, "w_k.png")),
    (Color.BLACK, PieceType.PAWN):   pygame.image.load(os.path.join(img_fldr, "b_p.png")),
    (Color.BLACK, PieceType.KNIGHT): pygame.image.load(os.path.join(img_fldr, "b_n.png")),
    (Color.BLACK, PieceType.BISHOP): pygame.image.load(os.path.join(img_fldr, "b_b.png")),
    (Color.BLACK, PieceType.ROOK):   pygame.image.load(os.path.join(img_fldr, "b_r.png")),
    (Color.BLACK, PieceType.QUEEN):  pygame.image.load(os.path.join(img_fldr, "b_q.png")),
    (Color.BLACK, PieceType.KING):   pygame.image.load(os.path.join(img_fldr, "b_k.png")),
}

def drawBoard(flipped=False):
    window.fill((0, 0, 0))
    window.blit(boardSurface, (10, 10))

    if selectedSquare is not None:
        row = selectedSquare // 8
        col = selectedSquare % 8
        if flipped:
            row = 7 - row
            col = 7 - col
        pygame.draw.rect(window, (255, 255, 0), (col * 60 + 10, row * 60 + 10, 60, 60), 4)

    for pieceType in [PieceType.PAWN, PieceType.KNIGHT, PieceType.BISHOP, PieceType.ROOK, PieceType.QUEEN, PieceType.KING]:
        for color in [Color.WHITE, Color.BLACK]:
            currentPiece = Piece(color, pieceType)
            
            # create a temporary copy so we dont delete pieces from engine
            rawBoard = chessBoard.getPieceBoard(currentPiece).getBoard()
            tempBoard = BitBoard(rawBoard)

            while tempBoard.getBoard():
                square = tempBoard.LS1B()
                tempBoard.flipBit(square)

                row = square // 8
                col = square % 8
                pieceImage = pieceImages[(color, pieceType)]

                drawX, drawY = 0, 0
                if flipped:
                    drawX = (7 - col) * 60 + 10
                    drawY = (7 - row) * 60 + 10
                else:
                    drawX = col * 60 + 10
                    drawY = row * 60 + 10
                
                window.blit(pieceImage, (drawX, drawY))

    pygame.display.flip()

def getSquareFromPos(pos, flipped):
    col = (pos[0] - 10) // 60
    row = (pos[1] - 10) // 60
    if col < 0 or col > 7 or row < 0 or row > 7: return None
    if flipped:
        col = 7 - col
        row = 7 - row
    return row * 8 + col

def pickColor():
    font = pygame.font.Font(None, 36)
    window.fill((50, 50, 50))
    whiteBtn = pygame.Rect(50, 200, 150, 50)
    blackBtn = pygame.Rect(300, 200, 150, 50)
    pygame.draw.rect(window, (200, 200, 200), whiteBtn)
    pygame.draw.rect(window, (100, 100, 100), blackBtn)
    window.blit(font.render('White', True, (0,0,0)), (90, 210))
    window.blit(font.render('Black', True, (255,255,255)), (340, 210))
    pygame.display.flip()

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if whiteBtn.collidepoint(event.pos): return Color.WHITE
                if blackBtn.collidepoint(event.pos): return Color.BLACK

playerColor = pickColor()
isWhiteTurn = True
flippedBoard = (playerColor == Color.WHITE)
selectedSquare = None
running = True

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        
        elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            if (isWhiteTurn and playerColor == Color.WHITE) or (not isWhiteTurn and playerColor == Color.BLACK):
                clickedSquare = getSquareFromPos(pygame.mouse.get_pos(), flippedBoard)
                
                if clickedSquare is not None:
                    if selectedSquare is None:
                        p = chessBoard.getPiece(clickedSquare)
                        if p is not None and int(p.color) == int(playerColor):
                            selectedSquare = clickedSquare
                    else:
                        moveList = chessBoard.generateMoves(playerColor)
                        foundMove = False
                        for m in moveList:
                            if m.startingSquare == selectedSquare and m.endSquare == clickedSquare:
                                chessBoard.makeAMove(m)
                                foundMove = True
                                break
                        
                        if foundMove:
                            isWhiteTurn = not isWhiteTurn
                            selectedSquare = None
                        else:
                            p = chessBoard.getPiece(clickedSquare)
                            if p is not None and int(p.color) == int(playerColor):
                                selectedSquare = clickedSquare
                            else:
                                selectedSquare = None

    engineTurn = (isWhiteTurn and playerColor == Color.BLACK) or (not isWhiteTurn and playerColor == Color.WHITE)
                 
    if engineTurn:
        pygame.display.flip() 
        result = Evaluation.search(chessBoard, 4, isWhiteTurn)
        chessBoard.makeAMove(result.bestMove)
        isWhiteTurn = not isWhiteTurn

    drawBoard(flippedBoard)
    pygame.time.delay(30)

pygame.quit()