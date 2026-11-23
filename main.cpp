#include <ncurses/curses.h>
#include <ctime>
#include <cstdlib>
#include "./head/menu.h"
#include "./head/game.h"

int main() {
    srand((unsigned)time(0));

    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    MENU menu;
    menu.initGame();

    bool running = true;
    while (running) {
        int pilihan = menu.menu();

        switch (pilihan) {
            case 0: { // PLAY
                clear();
                refresh();
                flushinp();
                Game start_game;
                start_game.game();
                break;
            }
            case 4: { // QUIT
                running = false;
                break;
            }
            default: {
                // Opsi lain belum ada isinya
                clear();
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(12, 8, "Fitur ini masih dalam pengerjaan, sabar ya...");
                attroff(COLOR_PAIR(2) | A_BOLD);
                refresh();
                napms(900);
                flushinp();
                break;
            }
        }
    }

    endwin();
    return 0;
}


