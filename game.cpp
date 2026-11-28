#ifdef _WIN32
#include <windows.h>
#endif

#include <ncurses/curses.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "./head/game.h"
#include "./head/audio.h"
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
    best_score = 0; // Masih rekor per-sesi, belum nyambung ke file highscore
    move_count = 0;
    game_over = false;
    game_won = false;
    keep_playing = false;
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

bool Game::canMove() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == 0) return true;

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE - 1; j++)
            if (board[i][j] == board[i][j + 1]) return true;

    for (int i = 0; i < SIZE - 1; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == board[i + 1][j]) return true;

    return false;
}

bool Game::checkWin() {
    if (keep_playing) return false;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == 2048) return true;
    return false;
}

// ============ GERAKAN TILE ============

// Slide & Merge per baris: padatkan -> gabung -> padatkan lagi
bool Game::slideRowLeft(int row[SIZE]) {
    bool moved = false;
    int temp[SIZE] = {0};
    int pos = 0;

    for (int i = 0; i < SIZE; i++)
        if (row[i] != 0) temp[pos++] = row[i];

    for (int i = 0; i < SIZE - 1; i++) {
        if (temp[i] != 0 && temp[i] == temp[i + 1]) {
            temp[i] *= 2;
            score += temp[i];
            temp[i + 1] = 0;
        }
    }

    int result[SIZE] = {0};
    pos = 0;
    for (int i = 0; i < SIZE; i++)
        if (temp[i] != 0) result[pos++] = temp[i];

    for (int i = 0; i < SIZE; i++) {
        if (row[i] != result[i]) moved = true;
        row[i] = result[i];
    }
    return moved;
}

bool Game::moveLeft() {
    bool moved = false;
    for (int i = 0; i < SIZE; i++)
        if (slideRowLeft(board[i])) moved = true;
    return moved;
}

bool Game::moveRight() {
    bool moved = false;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE / 2; j++) {
            int tmp = board[i][j];
            board[i][j] = board[i][SIZE - 1 - j];
            board[i][SIZE - 1 - j] = tmp;
        }
        if (slideRowLeft(board[i])) moved = true;
        for (int j = 0; j < SIZE / 2; j++) {
            int tmp = board[i][j];
            board[i][j] = board[i][SIZE - 1 - j];
            board[i][SIZE - 1 - j] = tmp;
        }
    }
    return moved;
}

bool Game::moveUp() {
    bool moved = false;
    for (int j = 0; j < SIZE; j++) {
        int col[SIZE];
        for (int i = 0; i < SIZE; i++) col[i] = board[i][j];
        if (slideRowLeft(col)) moved = true;
        for (int i = 0; i < SIZE; i++) board[i][j] = col[i];
    }
    return moved;
}

bool Game::moveDown() {
    bool moved = false;
    for (int j = 0; j < SIZE; j++) {
        int col[SIZE];
        for (int i = 0; i < SIZE; i++) col[i] = board[SIZE - 1 - i][j];
        if (slideRowLeft(col)) moved = true;
        for (int i = 0; i < SIZE; i++) board[SIZE - 1 - i][j] = col[i];
    }
    return moved;
}

void Game::processMove(bool moved) {
    if (moved) {
        move_count++;
        addRandomTile();

        if (score > best_score) {
            best_score = score;
        }

        if (checkWin()) {
            game_won = true;
        } else if (!canMove()) {
            game_over = true;
        }
    }
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

void Game::drawScore(int start_y, int score_x) {
    // Header Panel
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(start_y - 1, score_x, "==========================");
    mvprintw(start_y,     score_x, "    TUNGKU TRANSMUTASI    ");
    mvprintw(start_y + 1, score_x, "==========================");
    attroff(COLOR_PAIR(1) | A_BOLD);

    // Box Score Saat Ini
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(start_y + 3, score_x, "+------------------------+");
    mvprintw(start_y + 4, score_x, "|     SKOR SAAT INI      |");
    mvprintw(start_y + 5, score_x, "|     %8d           |", score);
    mvprintw(start_y + 6, score_x, "+------------------------+");
    attroff(COLOR_PAIR(2) | A_BOLD);

    // Box Rekor Tertinggi
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(start_y + 8,  score_x, "+------------------------+");
    mvprintw(start_y + 9,  score_x, "|    REKOR TERTINGGI     |");
    mvprintw(start_y + 10, score_x, "|      %8d          |", best_score);
    mvprintw(start_y + 11, score_x, "+------------------------+");
    attroff(COLOR_PAIR(5) | A_BOLD);

    // Detail Info Alkimia
    int highest_tile = getMaxTile();
    int filled = (SIZE * SIZE) - countEmpty();

    attron(COLOR_PAIR(1));
    mvprintw(start_y + 13, score_x, "     [ STATUS ALKIMIA ]");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(4));
    mvprintw(start_y + 14, score_x, "     Langkah     : %d", move_count);
    mvprintw(start_y + 15, score_x, "     Angka Max   : %d", highest_tile);
    mvprintw(start_y + 16, score_x, "     Grid Terisi : %d/%d", filled, SIZE * SIZE);
    mvprintw(start_y + 17, score_x, "     Ukuran Grid : 4x4 Matriks");
    mvprintw(start_y + 18, score_x, "     Target      : 2048 ");
    attroff(COLOR_PAIR(4));
}

void Game::drawInstructions(int start_y, int start_x, int cell_w, int cell_h) {
    int total_h = 1 + SIZE * (cell_h + 1);
    int y = start_y + total_h + 1;
    int board_w = 1 + SIZE * (cell_w + 1);

    attron(COLOR_PAIR(4));
    mvaddch(y, start_x, '+');
    for (int k = 1; k < board_w - 1; k++) mvaddch(y, start_x + k, '-');
    mvaddch(y, start_x + board_w - 1, '+');

    mvaddch(y + 1, start_x, '|');
    const char* line1 = "[Panah / WASD] Geser | [R] Reset | [Q] Menu";
    int l1_len = (int)strlen(line1);
    int l1_offset = (board_w - 2 > l1_len) ? (board_w - 2 - l1_len) / 2 : 1;
    mvprintw(y + 1, start_x + 1 + l1_offset, "%s", line1);
    mvaddch(y + 1, start_x + board_w - 1, '|');

    mvaddch(y + 2, start_x, '+');
    for (int k = 1; k < board_w - 1; k++) mvaddch(y + 2, start_x + k, '-');
    mvaddch(y + 2, start_x + board_w - 1, '+');
    attroff(COLOR_PAIR(4));
}

void Game::drawGameOver(int start_y, int start_x, int cell_w, int cell_h) {
    int total_h = 1 + SIZE * (cell_h + 1);
    int cy = start_y + (total_h - 5) / 2;
    int board_w = 1 + SIZE * (cell_w + 1);
    int cx = start_x + (board_w - 32) / 2;
    if (cx < 0) cx = 0;

    attron(COLOR_PAIR(6) | A_BOLD);
    mvprintw(cy,     cx, "+------------------------------+");
    mvprintw(cy + 1, cx, "|    TUNGKU MELEDAK BUNTU!     |");
    mvprintw(cy + 2, cx, "| Semua cawan penuh tersumbat! |");
    mvprintw(cy + 3, cx, "|  Tekan sembarang tombol...   |");
    mvprintw(cy + 4, cx, "+------------------------------+");
    attroff(COLOR_PAIR(6) | A_BOLD);
}

void Game::drawWin(int start_y, int start_x, int cell_w, int cell_h) {
    int total_h = 1 + SIZE * (cell_h + 1);
    int cy = start_y + (total_h - 5) / 2;
    int board_w = 1 + SIZE * (cell_w + 1);
    int cx = start_x + (board_w - 32) / 2;
    if (cx < 0) cx = 0;

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(cy,     cx, "+------------------------------+");
    mvprintw(cy + 1, cx, "|     MAGNUM OPUS 2048!        |");
    mvprintw(cy + 2, cx, "| Batu Bertuah Berhasil Dibuat!|");
    mvprintw(cy + 3, cx, "| [C] Lanjut [R] Baru [Q] Menu |");
    mvprintw(cy + 4, cx, "+------------------------------+");
    attroff(COLOR_PAIR(2) | A_BOLD);
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
    int total_w = board_w + 4 + 26; // Board + spasi + Score Panel
    int start_x = (max_x > total_w) ? (max_x - total_w) / 2 : 2;
    int start_y = (max_y >= 32) ? 3 : ((max_y >= 27) ? 2 : 1);
    int score_x = start_x + board_w + 3;

    drawBoard(start_y, start_x, cell_w, cell_h);
    drawScore(start_y, score_x);
    drawInstructions(start_y, start_x, cell_w, cell_h);

    if (game_over) drawGameOver(start_y, start_x, cell_w, cell_h);
    else if (game_won) drawWin(start_y, start_x, cell_w, cell_h);

    refresh();
}

// Game Loop
void Game::game() {
    // Putar musik gameplay
    playAudio("Musik/gameplay.wav");
    srand(time(NULL));

    initBoard();
    addRandomTile();
    addRandomTile();

    nodelay(stdscr, FALSE);
    flushinp();
    bool running = true;

    while (running) {
        render();
        int ch = getch();

        // Kondisi Menang: papan terkunci sampai pemain memilih
        if (game_won) {
            switch (ch) {
                case 'c': case 'C':
                    keep_playing = true;
                    game_won = false;
                    continue;
                case 'r': case 'R':
                    initBoard();
                    addRandomTile();
                    addRandomTile();
                    continue;
                case 'q': case 'Q':
                    running = false;
                    continue;
                default:
                    continue;
            }
        }

        // Kontrol Pergerakan
        bool moved = false;
        switch (ch) {
            case KEY_UP:    case 'w': case 'W':
                moved = moveUp(); break;
            case KEY_DOWN:  case 's': case 'S':
                moved = moveDown(); break;
            case KEY_LEFT:  case 'a': case 'A':
                moved = moveLeft(); break;
            case KEY_RIGHT: case 'd': case 'D':
                moved = moveRight(); break;
            case 'r': case 'R':
                initBoard();
                addRandomTile();
                addRandomTile();
                continue;
            case 'q': case 'Q':
                running = false;
                continue;
        }

        processMove(moved);

        // Kalau langkah ini bikin buntu: pajang papan buntu sejenak, lalu balik menu
        if (game_over) {
            render();
            napms(500);
            flushinp();
            getch();
            running = false;
        }
    }

    stopAudio();
    clear();
    refresh();
}