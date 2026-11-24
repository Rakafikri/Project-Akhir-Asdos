#include <ncurses/curses.h>
#include "./head/menu.h"
using namespace std;

void MENU::initGame(){
    setlocale(LC_ALL, "");

    if(has_colors()){
        start_color();
        use_default_colors();

        init_pair(1, COLOR_CYAN, -1);    // Judul
        init_pair(2, COLOR_WHITE, -1);   // Teks Normal
        init_pair(3, COLOR_BLACK, COLOR_CYAN); // Highlight Menu
        init_pair(4, COLOR_YELLOW, -1);  // Border
        init_pair(5, COLOR_GREEN, -1);   // High Score
        init_pair(6, COLOR_RED, -1);     // Quit / Warning

        // Warna tile 2048
        init_pair(10, 8, -1);   // 0: Kosong
        init_pair(11, 250, -1); // 2
        init_pair(12, 255, -1); // 4
        init_pair(13, 222, -1); // 8
        init_pair(14, 214, -1); // 16
        init_pair(15, 208, -1); // 32
        init_pair(16, 196, -1); // 64
        init_pair(17, 228, -1); // 128
        init_pair(18, 226, -1); // 256
        init_pair(19, 220, -1); // 512
        init_pair(20, 48, -1);  // 1024
        init_pair(21, 46, -1);  // 2048
        init_pair(22, 201, -1); // 4096+
    }
}

void MENU::tampilanJudul(int start_y, int start_x) {
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(start_y, start_x, "  .d8888b.   .d8888b.      d8888   .d8888b.  ");
    mvprintw(start_y + 1, start_x, " d88P  Y88b d88P  Y88b    d8P888  d88P  Y88b ");
    mvprintw(start_y + 2, start_x, "        888 888    888   d8P 888  Y88b. d88P ");
    mvprintw(start_y + 3, start_x, "      .d88P 888    888  d8P  888   \"Y88888\"  ");
    mvprintw(start_y + 4, start_x, "  .od888P\"  888    888 d88   888  .d8P\"\"Y8b. ");
    mvprintw(start_y + 5, start_x, " d88P\"      888    888 8888888888 888    888 ");
    mvprintw(start_y + 6, start_x, " 888\"       Y88b  d88P       888  Y88b  d88P ");
    mvprintw(start_y + 7, start_x, " 888888888   \"Y8888P\"        888   \"Y8888P\"  ");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(4));
    mvprintw(start_y + 9, start_x + 8, "~*~ THE ALCHEMY 4x4 ~*~");
    attroff(COLOR_PAIR(4));
}

void MENU::tampilanMenu(int highlight) {
    const char *opsi[] = {
        "PLAY GAME",
        "CONTINUE",
        "STORY",
        "HIGH SCORE",
        "QUIT"
    };

    int jumlah_opsi = 5;

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int box_width = 34;
    int box_height = 9;
    int box_x = (max_x - box_width) / 2;
    int box_y = 15;

    // Tampilkan Judul
    int judul_x = (max_x - 44) / 2;
    tampilanJudul(2, judul_x);

    // Gambar Kotak Luar Menu
    attron(COLOR_PAIR(2));
    for(int i = 0; i < box_height; i++) {
        for(int j = 0; j < box_width; j++) {
            if(i == 0 && j == 0) mvaddch(box_y + i, box_x + j, ACS_ULCORNER);
            else if(i == 0 && j == box_width - 1) mvaddch(box_y + i, box_x + j, ACS_URCORNER);
            else if(i == box_height - 1 && j == 0) mvaddch(box_y + i, box_x + j, ACS_LLCORNER);
            else if(i == box_height - 1 && j == box_width - 1) mvaddch(box_y + i, box_x + j, ACS_LRCORNER);
            else if(i == 0 || i == box_height - 1) mvaddch(box_y + i, box_x + j, ACS_HLINE);
            else if(j == 0 || j == box_width - 1) mvaddch(box_y + i, box_x + j, ACS_VLINE);
        }
    }
    attroff(COLOR_PAIR(2));

    // Opsi di dalam kotak
    for(int i = 0; i < jumlah_opsi; i++) {
        int text_x = box_x + (box_width - 20) / 2;
        int text_y = box_y + 2 + i;

        if(highlight == i) {
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(text_y, text_x, " -> % -13s ", opsi[i]);
            attroff(COLOR_PAIR(3) | A_BOLD);
        } else {
            attron(COLOR_PAIR(2));
            mvprintw(text_y, text_x, "    % -13s ", opsi[i]);
            attroff(COLOR_PAIR(2));
        }
    }

    // Hint
    attron(A_DIM);
    mvprintw(box_y + box_height + 2, (max_x - 42) / 2, "Gunakan [Panah Atas/Bawah] dan [ENTER]");
    attroff(A_DIM);
}

int MENU::menu() {
    int highlight = 0;
    int pilihan = -1;
    int input;

    while(1) {
        clear();
        tampilanMenu(highlight);
        refresh();

        input = getch();

        switch(input) {
            case KEY_UP:
            case 'w':
            case 'W':
                highlight = (highlight == 0) ? 4 : highlight - 1;
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                highlight = (highlight == 4) ? 0 : highlight + 1;
                break;
            case 10:
            case '\n':
            case '\r':
            case KEY_ENTER:
                pilihan = highlight;
                return pilihan;
            default:
                break;
        }
    }
}