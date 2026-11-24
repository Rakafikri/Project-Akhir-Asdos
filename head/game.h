#ifndef GAME_H
#define GAME_H

class Game {
public:
    static const int SIZE = 4; // Grid 4x4
    void game();

private:
    int board[SIZE][SIZE];
    int score;
    int move_count;
    bool game_over;

    void initBoard();
    void addRandomTile();
    int  countEmpty();
    int  getMaxTile();

    // Gerakan
    void transpose();
    void reverseRows();
    bool slideLeft();
    bool slideRight();
    bool slideUp();
    bool slideDown();
    bool move(int dir);

    // Rendering
    int  getColorPair(int value);
    void drawCell(int row, int col, int cy, int cx, int cell_w, int cell_h);
    void drawBoard(int start_y, int start_x, int cell_w, int cell_h);
    void render();
};

typedef Game Game2048;

#endif