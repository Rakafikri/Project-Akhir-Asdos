#include <ncurses/curses.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <utility>
#include "./head/game.h"
using namespace std;

// Color Mapping untuk tile 2048
int Game::getColorPair(int value) {
    switch (value) {
        case 0:    return 10;
        case 2:    return 11;
        case 4:    return 12;
        case 8:    return 13;
        case 16:   return 14;
        case 32:   return 15;
        case 64:   return 16;
        case 128:  return 17;
        case 256:  return 18;
        case 512:  return 19;
        case 1024: return 20;
        case 2048: return 21;
        default:   return 22;
    }
}

int Game::getMaxTile() {
    int max_val = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] > max_val) {
                max_val = board[i][j];
            }
        }
    }
    return max_val;
}

// ============ LOGIKA BOARD 4x4 ============

void Game::initBoard() {
    memset(board, 0, sizeof(board));
    score = 0;
    move_count = 0;
    game_over = false;
}

int Game::countEmpty() {
    int count = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == 0) count++;
    return count;
}

void Game::addRandomTile() {
    int empty = countEmpty();
    if (empty == 0) return;

    int target = rand() % empty;
    int count = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                if (count == target) {
                    board[i][j] = (rand() % 10 == 0) ? 4 : 2;
                    return;
                }
                count++;
            }
        }
    }
}

// ============ GERAKAN TILE ============

void Game::transpose() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = i + 1; j < SIZE; j++) {
            std::swap(board[i][j], board[j][i]);
        }
    }
}

void Game::reverseRows() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE / 2; j++) {
            std::swap(board[i][j], board[i][SIZE - 1 - j]);
        }
    }
}

bool Game::slideLeft() {
    bool moved = false;
    for (int i = 0; i < SIZE; i++) {
        // 1) Kompresi: buang nol
        int temp[SIZE] = {0};
        int idx = 0;
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != 0) {
                temp[idx++] = board[i][j];
            }
        }

        // 2) Gabungkan tile yang nilainya sama
        for (int j = 0; j < SIZE - 1; j++) {
            if (temp[j] != 0 && temp[j] == temp[j + 1]) {
                temp[j] *= 2;
                score += temp[j];
                temp[j + 1] = 0;
            }
        }

        // 3) Tulis balik ke board
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != temp[j]) moved = true;
            board[i][j] = temp[j];
        }
    }
    return moved;
}

bool Game::slideRight() {
    reverseRows();
    bool moved = slideLeft();
    reverseRows();
    return moved;
}

bool Game::slideUp() {
    transpose();
    bool moved = slideLeft();
    transpose();
    return moved;
}

bool Game::slideDown() {
    transpose();
    reverseRows();
    bool moved = slideLeft();
    reverseRows();
    transpose();
    return moved;
}

bool Game::move(int dir) {
    bool moved = false;
    if (dir == 0) moved = slideUp();
    else if (dir == 1) moved = slideRight();
    else if (dir == 2) moved = slideDown();
    else if (dir == 3) moved = slideLeft();
    return moved;
}

// ============ RENDERING (UKURAN GRID 4x4 KOTAK SQUARE) ============

void Game::drawCell(int row, int col, int cy, int cx, int cell_w, int cell_h) {
    int value = board[row][col];
    int color = getColorPair(value);

    // Aktifkan warna dan cetak tebal (A_BOLD) untuk warna mencolok & kontras tinggi
    attron(COLOR_PAIR(color) | A_BOLD);

    // Warnai penuh seluruh area kotak (cell_w karakter x cell_h baris)
    for (int dy = 0; dy < cell_h; dy++) {
        for (int k = 0; k < cell_w; k++) {
            mvaddch(cy + dy, cx + k, ' ');
        }
    }

    // Teks angka atau titik diposisikan tepat di baris tengah kotak
    int text_y = cy + (cell_h - 1) / 2;
    if (value != 0) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", value);
        int len = (int)strlen(buf);
        int offset = (cell_w - len) / 2;
        mvprintw(text_y, cx + offset, "%s", buf);
    } else {
        // Titik murni ASCII '.' tepat di tengah kotak
        int offset = (cell_w - 1) / 2;
        mvprintw(text_y, cx + offset, ".");
    }
    attroff(COLOR_PAIR(color) | A_BOLD);
}

void Game::drawBoard(int start_y, int start_x, int cell_w, int cell_h) {
    int board_w = 1 + SIZE * (cell_w + 1);

    // Judul Grid 4x4 di atas papan (centered)
    const char* title = "=== TUNGKU TRANSMUTASI 4x4 ===";
    int title_len = (int)strlen(title);
    int title_offset = (board_w > title_len) ? (board_w - title_len) / 2 : 0;
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(start_y - 2, start_x + title_offset, "%s", title);
    attroff(COLOR_PAIR(1) | A_BOLD);

    // Border atas
    attron(COLOR_PAIR(1));
    mvaddch(start_y, start_x, '+');
    for (int j = 0; j < SIZE; j++) {
        for (int k = 0; k < cell_w; k++) addch('=');
        addch('+');
    }
    attroff(COLOR_PAIR(1));

    // Render 4 baris sel dengan ukuran kotak bujur sangkar (square)
    for (int i = 0; i < SIZE; i++) {
        int cell_y = start_y + 1 + i * (cell_h + 1);
        int div_y  = cell_y + cell_h;

        // Border kiri untuk setiap baris
        for (int dy = 0; dy < cell_h; dy++) {
            attron(COLOR_PAIR(1));
            mvaddch(cell_y + dy, start_x, '|');
            attroff(COLOR_PAIR(1));
        }

        // Render setiap sel dalam baris
        for (int j = 0; j < SIZE; j++) {
            int cell_x = start_x + 1 + j * (cell_w + 1);

            drawCell(i, j, cell_y, cell_x, cell_w, cell_h);

            for (int dy = 0; dy < cell_h; dy++) {
                attron(COLOR_PAIR(1));
                mvaddch(cell_y + dy, cell_x + cell_w, '|');
                attroff(COLOR_PAIR(1));
            }
        }

        // Garis pemisah horizontal di bawah baris i
        attron(COLOR_PAIR(1));
        mvaddch(div_y, start_x, '+');
        char border_char = (i == SIZE - 1) ? '=' : '-';
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < cell_w; k++) addch(border_char);
            addch('+');
        }
        attroff(COLOR_PAIR(1));
    }
}

void Game::render() {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Ukuran kotak bujur sangkar (Square) proporsional 4x4
    int cell_h = 5;
    int cell_w = 11;

    if (max_y < 27) {
        cell_h = 3;
        cell_w = 7;
    } else if (max_y < 32) {
        cell_h = 4;
        cell_w = 9;
    }

    int board_w = 1 + SIZE * (cell_w + 1);
    int start_x = (max_x > board_w + 4) ? (max_x - board_w) / 2 : 2;
    int start_y = (max_y >= 32) ? 3 : ((max_y >= 27) ? 2 : 1);

    drawBoard(start_y, start_x, cell_w, cell_h);

    // Hint bawah papan
    int hint_y = start_y + 1 + SIZE * (cell_h + 1) + 1;
    attron(A_DIM);
    mvprintw(hint_y, start_x, "[Panah/WASD] Geser   [Q] Keluar");
    attroff(A_DIM);

    refresh();
}

// Game Loop
void Game::game() {
    initBoard();
    addRandomTile();
    addRandomTile();

    while (true) {
        render();
        int ch = getch();

        bool moved = false;
        if (ch == KEY_UP || ch == 'w' || ch == 'W') moved = move(0);
        else if (ch == KEY_RIGHT || ch == 'd' || ch == 'D') moved = move(1);
        else if (ch == KEY_DOWN || ch == 's' || ch == 'S') moved = move(2);
        else if (ch == KEY_LEFT || ch == 'a' || ch == 'A') moved = move(3);
        else if (ch == 'q' || ch == 'Q') break;

        if (moved) {
            addRandomTile();
        }
    }

    clear();
    refresh();
}