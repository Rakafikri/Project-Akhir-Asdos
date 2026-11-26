#ifndef GAME_H
#define GAME_H

class Game {
public:
    static const int SIZE = 4; // Grid 4x4
    void game();

private:
    int board[SIZE][SIZE];
    int score;
    int best_score;
    int move_count;
    bool game_over;
    bool game_won;
    bool keep_playing;

    void initBoard();
    void addRandomTile();
    int  countEmpty();
    int  getMaxTile();
    bool canMove();
    bool checkWin();

    // Gerakan
    bool slideRowLeft(int row[SIZE]);
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    void processMove(bool moved);

    // Rendering
    int  getColorPair(int value);
    void drawCell(int row, int col, int cy, int cx, int cell_w, int cell_h);
    void drawBoard(int start_y, int start_x, int cell_w, int cell_h);
    void drawScore(int start_y, int score_x);
    void drawInstructions(int start_y, int start_x, int cell_w, int cell_h);
    void drawGameOver(int start_y, int start_x, int cell_w, int cell_h);
    void drawWin(int start_y, int start_x, int cell_w, int cell_h);
    void render();
};

typedef Game Game2048;

#endif