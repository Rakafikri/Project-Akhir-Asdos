#ifdef _WIN32
#include <windows.h>
#endif

#include <ncurses/curses.h>
#include "./head/story.h"
#include "./head/game.h"
#include "./head/menu.h"
#include "./head/audio.h"
using namespace std;

int main() {
    initscr();
    curs_set(0);
    noecho();

    MENU menu;
    menu.initGame();

    playAudio("Musik/lobby.wav");

    // Inisialisasi tampilan awal judul & animasi loading
    Story story;
    story.Title();
    story.Loading();
    clear();
    refresh();

    bool running = true;
    bool first_play = true;
    while (running) {
        int pilihan = menu.menu();

        switch (pilihan) {
            case 0: { // PLAY
                clear();
                refresh();
                flushinp();
                if (first_play) {
                    nodelay(stdscr, TRUE);
                    story.reset_skipped();
                    story.Intro();
                    first_play = false;
                    clear();
                    refresh();
                    flushinp();
                }
                Game start_game;
                start_game.game();
                break;
            }
            case 2: { // STORY
                clear();
                refresh();
                flushinp();
                nodelay(stdscr, TRUE);
                story.reset_skipped();
                story.Intro();
                clear();
                refresh();
                flushinp();
                break;
            }
            case 4: { // QUIT
                running = false;
                break;
            }
            default: {
                // CONTINUE / HIGH SCORE masih placeholder
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

    stopAudio();
    endwin();
    return 0;
}