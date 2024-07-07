@echo off
setlocal enabledelayedexpansion

set depth=%1
set fen=%2

rem Path to your compiled chess engine executable
set chess_engine=build\chess_engine.exe

rem Run your chess engine with the given parameters and capture the output
echo setboard %fen% > input.txt
echo perft %depth% >> input.txt
%chess_engine% < input.txt > output.txt

rem Process the output to match the format expected by perftree
findstr "^[a-h][1-8][a-h][1-8]" output.txt
echo.
findstr "^[0-9]" output.txt

del input.txt
del output.txt
