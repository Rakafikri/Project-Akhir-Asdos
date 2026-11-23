#include <ncurses/curses.h>
#include <cstring>
#include <string>
#include "./head/menu.h"
using namespace std;

void MENU::initGame() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLUE);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
}

// Judul Game 2048 (ditaruh di tengah layar)
void MENU::tampilanJudul() {
    int max_x = getmaxx(stdscr);
    int cx = (max_x > 32) ? (max_x - 32) / 2 : 5;
    int awalX = 4;

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(awalX,     cx, "    ____    ___   _  _    ___  ");
    mvprintw(awalX + 1, cx, "   |___ \\  / _ \\ | || |  ( _ ) ");
    mvprintw(awalX + 2, cx, "     __) || | | || || |_ / _ \\ ");
    mvprintw(awalX + 3, cx, "    / __/ | |_| ||__   _| (_) |");
    mvprintw(awalX + 4, cx, "   |_____| \\___/    |_|  \\___/ ");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(2) | A_BOLD);
    const char* sub = "~*~ THE ALCHEMY 4x4 ~*~";
    int sub_len = (int)strlen(sub);
    mvprintw(awalX + 6, (max_x > sub_len) ? (max_x - sub_len) / 2 : 5, "%s", sub);
    attroff(COLOR_PAIR(2) | A_BOLD);
}

// Menu Navigasi (5 Opsi) - Ditengahkan Secara Simetris
void MENU::tampilanMenu(int pilihan) {
    int max_x = getmaxx(stdscr);
    short int box_w = 34;
    short int kolom_kiriAtas = (max_x > box_w) ? (max_x - box_w) / 2 : 23;
    short int baris_kiriAtas = 12;
    short int kolom_kananBawah = kolom_kiriAtas + box_w;
    short int baris_kananBawah = baris_kiriAtas + 12;
    short int kolom, baris;

    // Bingkai Box Menu
    attron(COLOR_PAIR(1));
    mvaddch(baris_kiriAtas, kolom_kiriAtas, ACS_ULCORNER);
    for (kolom = kolom_kiriAtas + 1; kolom < kolom_kananBawah; kolom++) mvaddch(baris_kiriAtas, kolom, ACS_HLINE);
    mvaddch(baris_kiriAtas, kolom_kananBawah, ACS_URCORNER);

    for (baris = baris_kiriAtas + 1; baris < baris_kananBawah; baris++) {
        mvaddch(baris, kolom_kiriAtas, ACS_VLINE);
        mvaddch(baris, kolom_kananBawah, ACS_VLINE);
    }

    mvaddch(baris_kananBawah, kolom_kananBawah, ACS_LRCORNER);
    for (kolom = kolom_kananBawah - 1; kolom > kolom_kiriAtas; kolom--) mvaddch(baris_kananBawah, kolom, ACS_HLINE);
    mvaddch(baris_kananBawah, kolom_kiriAtas, ACS_LLCORNER);
    attroff(COLOR_PAIR(1));

    const char* menuOpsi[5];
    menuOpsi[0] = "PLAY";
    menuOpsi[1] = "CONTINUE";
    menuOpsi[2] = "STORY";
    menuOpsi[3] = "HIGH SCORE";
    menuOpsi[4] = "QUIT";

    for (int i = 0; i < 5; i++) {
        int baris_teks = baris_kiriAtas + 1 + i * 2;
        int panjang_opsi = (int)string(menuOpsi[i]).length();
        int offset_x = (box_w - panjang_opsi) / 2;

        if (pilihan == i) {
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(baris_teks, kolom_kiriAtas + 1, "                                ");
            mvprintw(baris_teks, kolom_kiriAtas + offset_x - 2, "> %s <", menuOpsi[i]);
            attroff(COLOR_PAIR(3) | A_BOLD);
        } else {
            attron(COLOR_PAIR(4));
            mvprintw(baris_teks, kolom_kiriAtas + 1, "                                ");
            mvprintw(baris_teks, kolom_kiriAtas + offset_x, "%s", menuOpsi[i]);
            attroff(COLOR_PAIR(4));
        }
    }

    // Hint tombol navigasi
    attron(A_DIM);
    const char* hint = "Gunakan Panah / WASD  |  Enter: Pilih";
    int hint_x = (max_x > 36) ? (max_x - 36) / 2 : 22;
    mvprintw(baris_kananBawah + 1, hint_x, "%s", hint);
    attroff(A_DIM);
}

// Loop Utama Menu
int MENU::menu() {
    nodelay(stdscr, FALSE);

    int pilihan = 0;
    int ch;

    while (true) {
        clear();
        tampilanJudul();
        tampilanMenu(pilihan);
        refresh();

        ch = getch();
        if (ch == KEY_UP || ch == 'w' || ch == 'W') {
            pilihan--;
            if (pilihan < 0) pilihan = 4;
        }
        else if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
            pilihan++;
            if (pilihan > 4) pilihan = 0;
        }
        else if (ch == 10 || ch == 13 || ch == KEY_ENTER || ch == '\n' || ch == '\r') {
            flushinp();
            return pilihan;
        }
    }
}