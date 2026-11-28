#ifdef _WIN32
#include <windows.h>
#endif

#include <ncurses/curses.h>
#include "./head/story.h"
#include "./head/menu.h"
#include "./head/audio.h"
#include <iostream>
using namespace std;

// Efek suara ketikan (typewriter click)
void Story::playTypeClick() {
#ifdef _WIN32
    playTypewriterSFX();
#else
    beep();
#endif
}

bool Story::input() {
    nodelay(stdscr, TRUE);
    int ch = getch();
    if (ch != ERR && (ch == 's' || ch == 'S' || ch == 27)) {
        stopAudio();
        closeTypewriterSFX();
        _skipped = true;
        return true;
    }
    return false;
}

void Story::wait_or_skip(int ms) {
    if (_skipped) return;
    for (int t = 0; t < ms; t += 40) {
        napms(40);
        if (input()) return;
    }
}

void Story::reciter(string text, int delay) {
    if (_skipped) return;
    int char_count = 0;
    for (char c : text) {
        if (_skipped) return;
        addch(c);
        refresh();
        char_count++;
        if (c != ' ' && c != '\n' && char_count % 2 == 0) {
            playTypeClick();
        }
        napms(delay);
        if (input()) return;
    }
}

void Story::drawSkipHint() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    attron(A_DIM);
    mvprintw(max_y - 2, (max_x > 32) ? max_x - 32 : 2, "[ Tekan 'S' untuk Lewati ]");
    attroff(A_DIM);
}

// Judul game
void Story::Title() {
    curs_set(0);
    clear();

    int max_x = getmaxx(stdscr);
    int cx = (max_x > 32) ? (max_x - 32) / 2 : 5;

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(6,  cx, "  ____    ___   _  _    ___  ");
    mvprintw(7,  cx, " |___ \\  / _ \\ | || |  ( _ ) ");
    mvprintw(8,  cx, "   __) || | | || || |_ / _ \\ ");
    mvprintw(9,  cx, "  / __/ | |_| ||__   _| (_) |");
    mvprintw(10, cx, " |_____| \\___/    |_|  \\___/ ");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(12, (max_x - 34) / 2, "      ~*~ ALKIMIA  4x4 ~*~");
    attroff(COLOR_PAIR(2) | A_BOLD);

    refresh();

    for (int t = 0; t < 2000; t += 100) {
        napms(100);
        nodelay(stdscr, TRUE);
        if (input()) return;
    }
}

// Tampilan loading
void Story::Loading() {
    curs_set(0);
    clear();

    int max_x = getmaxx(stdscr);
    int cx = (max_x > 25) ? (max_x - 25) / 2 : 25;

    for (int i = 0; i < 2; i++) {
        mvprintw(15, cx, "   Inisialisasi... ");
        mvprintw(16, cx, "-------------------- ");
        mvprintw(17, cx, "|                   |");
        mvprintw(18, cx, "-------------------- ");
        mvprintw(19, cx, "  Menyiapkan Tungku ");

        for (int f = 1; f <= 19; f++) {
            mvprintw(17, cx + f, "|");
            refresh();
            napms(35);
            nodelay(stdscr, TRUE);
            if (input()) return;
        }
    }
}

// Story: Legenda Alkemis & Tungku Transmutasi Batu Bertuah 4x4
void Story::Intro() {
    // Putar musik narasi story dan inisialisasi efek suara ketikan
    playAudio("Musik/story.wav");
    initTypewriterSFX();

    curs_set(0);
    _skipped = false;
    nodelay(stdscr, TRUE);
    flushinp(); // Sangat penting: bersihkan sisa keystroke Enter dari menu

    clear();
    refresh();

    int max_x = getmaxx(stdscr);
    int cx = (max_x > 75) ? (max_x - 75) / 2 : 2;

    // ====== SCENE 1: Menara Alkemis Kuno & Kitab Tabula Smaragdina ======
    drawSkipHint();
    attron(COLOR_PAIR(1));
    mvprintw(3,  cx + 8, "              /\\           *         .             .       ");
    mvprintw(4,  cx + 8, "             /  \\      .        *           *         .    ");
    mvprintw(5,  cx + 8, "            /____\\           .        .          *         ");
    mvprintw(6,  cx + 8, "           | [  ] |    *         .          .        .     ");
    mvprintw(7,  cx + 8, "           |  ||  |         .          *        .          ");
    mvprintw(8,  cx + 8, "          /________\\    ~ THE ANCIENT TOWER OF ALCHEMY ~   ");
    attroff(COLOR_PAIR(1));
    refresh();

    wait_or_skip(1200);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(4));
    move(11, cx);
    reciter("Di puncak Menara Alkemis Kuno, terukir manuskrip terlarang para leluhur:");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(2) | A_BOLD);
    move(13, cx);
    reciter("TABULA SMARAGDINA — Rahasia Besar Penciptaan Kristal Abadi: 2048.");
    attroff(COLOR_PAIR(2) | A_BOLD);
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(4));
    move(15, cx);
    reciter("Legenda mencatat: barangsiapa mampu melebur unsur hingga mencapai kemurnian 2048...");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(5) | A_BOLD);
    move(17, cx);
    reciter("Akan membangkitkan BATU BERTUAH (AURUM MAGNUM) yang mengubah segala logam jadi emas!");
    attroff(COLOR_PAIR(5) | A_BOLD);
    attroff(COLOR_PAIR(4));
    wait_or_skip(1600);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    clear();
    napms(150);

    // ====== SCENE 2: Altar Transmutasi 4x4 (16 Cawan Peleburan Suci) ======
    drawSkipHint();
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(3,  cx + 10, "+-----------+-----------+-----------+-----------+");
    mvprintw(4,  cx + 10, "| (2)Timbal |     .     |  (4)Besi  |     .     |");
    mvprintw(5,  cx + 10, "+-----------+-----------+-----------+-----------+");
    mvprintw(6,  cx + 10, "|     .     | (8)Tembaga|     .     | (16)Perak |");
    mvprintw(7,  cx + 10, "+-----------+-----------+-----------+-----------+");
    mvprintw(8,  cx + 10, "|  (4)Besi  |     .     |(32)Merkuri| (8)Tembaga|");
    mvprintw(9,  cx + 10, "+-----------+-----------+-----------+-----------+");
    mvprintw(10, cx + 10, "|     .     | (64)Kobalt|     .     | (128)Intan|");
    mvprintw(11, cx + 10, "+-----------+-----------+-----------+-----------+");
    attroff(COLOR_PAIR(1) | A_BOLD);
    refresh();

    wait_or_skip(1200);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(4));
    move(13, cx);
    reciter("Di hadapanmu terbentang Altar Transmutasi 4x4: persis 16 cawan peleburan suci.");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    move(15, cx);
    reciter("Setiap cawan menampung inti logam: bernilai 2, 4, 8, 16, hingga ratusan.");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(2) | A_BOLD);
    move(17, cx);
    reciter("Reaksi di altar ini sangat sensitif — hanya alkemis berdisiplin yang bisa menguasainya!");
    attroff(COLOR_PAIR(2) | A_BOLD);
    attroff(COLOR_PAIR(4));
    wait_or_skip(1600);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    clear();
    napms(150);

    // ====== SCENE 3: Hukum Peleburan Logam & Kendali Altar ======
    drawSkipHint();
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(3,  cx + 8, "     [ TIMBAL : 2 ]   ==== MANTRA ====>   <==== MANTRA ====   [ TIMBAL : 2 ]     ");
    mvprintw(4,  cx + 8, "                                       ||                                        ");
    mvprintw(5,  cx + 8, "                                       vv                                        ");
    mvprintw(6,  cx + 8, "                             [ BESI MURNI : 4 ]                                  ");
    mvprintw(7,  cx + 8, "                        ~ HUKUM TRANSMUTASI GANDA ~                              ");
    attroff(COLOR_PAIR(2) | A_BOLD);
    refresh();

    wait_or_skip(1200);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(4));
    move(9, cx);
    reciter("Sebagai Murid Alkemis Terpilih, kamu mengendalikan tiupan angin tungku.");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    move(11, cx);
    reciter("Gunakan [Panah / WASD] untuk menggeser seluruh unsur logam meluncur di atas altar.");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    move(13, cx);
    reciter("Dua logam sejenis yang bertubrukan akan bereaksi dan bertransmutasi jadi logam baru!");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(2) | A_BOLD);
    move(15, cx);
    reciter("2 jadi 4... 4 jadi 8... hingga mencapai intisari termurni: AURUM 2048!");
    attroff(COLOR_PAIR(2) | A_BOLD);

    move(17, cx);
    reciter("Namun ingat: setiap hembusan mantra akan memunculkan serbuk unsur baru di cawan kosong.");
    attroff(COLOR_PAIR(4));
    wait_or_skip(1600);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    clear();
    napms(150);

    // ====== SCENE 4: Peringatan Meluapnya Tungku (16 Cawan Penuh) ======
    drawSkipHint();
    attron(COLOR_PAIR(6) | A_BOLD);
    mvprintw(3,  cx + 8, "/!\\ PERINGATAN ALKEMIS: BATAS MAKSIMAL 16 CAWAN /!\\");
    mvprintw(4,  cx + 8, "+----------------------------------------------------+");
    mvprintw(5,  cx + 8, "|   KAPASITAS ALTAR TERBATAS: HANYA 16 TEMPAT!       |");
    mvprintw(6,  cx + 8, "|    JANGAN BIARKAN CAWAN MACET DAN TERSUMBAT!       |");
    mvprintw(7,  cx + 8, "+----------------------------------------------------+");
    attroff(COLOR_PAIR(6) | A_BOLD);
    refresh();

    wait_or_skip(1200);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(4));
    move(9, cx);
    reciter("Waspadalah! Jika ke-16 cawan terisi penuh dan tak ada lagi unsur yang bisa dileburkan...");
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(6) | A_BOLD);
    move(11, cx);
    reciter("Suhu tungku meluap tak terkendali -> MELEDAK JADI ABU (GAME OVER)!");
    attroff(COLOR_PAIR(6) | A_BOLD);
    wait_or_skip(900);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    attron(COLOR_PAIR(2) | A_BOLD);
    move(13, cx);
    reciter("Fokuskan pikiranmu, susun strategi transmutasi, dan tuntaskan Kitab Alkimia 2048!");
    attroff(COLOR_PAIR(2) | A_BOLD);
    attroff(COLOR_PAIR(4));
    wait_or_skip(1600);
    if (_skipped) { stopAudio(); closeTypewriterSFX(); return; }

    clear();
    napms(200);

    move(10, (max_x > 54) ? (max_x - 54) / 2 : 5);
    attron(COLOR_PAIR(1) | A_BOLD);
    reciter("> Tungku Transmutasi 4x4 telah dinyalakan... Mulailah meneliti!");
    attroff(COLOR_PAIR(1) | A_BOLD);
    refresh();
    wait_or_skip(1200);

    stopAudio();
    closeTypewriterSFX();
    nodelay(stdscr, FALSE);
    flushinp();
    clear();
    refresh();
}
