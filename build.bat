@echo off
g++ main.cpp menu.cpp game.cpp story.cpp -o program.exe -std=c++17 -lncurses -lwinmm -DNCURSES_STATIC
if %errorlevel% == 0 (
    echo Build sukses!
    program.exe
) else (
    echo Build gagal.
)