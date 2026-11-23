#include <ncurses/curses.h>
#include "./head/menu.h"

int main() {
    initscr();
    curs_set(0);
    noecho();

    MENU menu;
    menu.initGame();

    bool running = true;
    while (running) {
        int pilihan = menu.menu();

        if (pilihan == 4) { // QUIT
            running = false;
        } else {
            clear();
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(12, 8, "Fitur ini masih dalam pengerjaan");
            attroff(COLOR_PAIR(2) | A_BOLD);
            refresh();
            napms(900);
            flushinp();
        }
    }

    endwin();
    return 0;
}