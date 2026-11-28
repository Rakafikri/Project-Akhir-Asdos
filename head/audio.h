#ifndef AUDIO_H
#define AUDIO_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>

#ifndef SND_ASYNC
#define SND_ASYNC 0x00000001
#endif
#ifndef SND_LOOP
#define SND_LOOP 0x00000008
#endif
#ifndef SND_FILENAME
#define SND_FILENAME 0x00020000
#endif
#endif

#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>

inline std::string resolveAudioPath(const char* filepath) {
#ifdef _WIN32
    if (!filepath || strlen(filepath) == 0) return "";

    char exePath[MAX_PATH] = {0};
    std::string exeDir = "";
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) > 0) {
        char* lastSlash = strrchr(exePath, '\\');
        if (!lastSlash) lastSlash = strrchr(exePath, '/');
        if (lastSlash) {
            *lastSlash = '\0';
            exeDir = exePath;
        }
    }

    const char* filenameOnly = filepath;
    const char* slash = strrchr(filepath, '/');
    if (!slash) slash = strrchr(filepath, '\\');
    if (slash) filenameOnly = slash + 1;

    std::string candidates[] = {
        std::string(filepath),
        exeDir + "\\Sound\\" + filenameOnly,
        exeDir + "\\..\\Sound\\" + filenameOnly,
        std::string("Sound/") + filenameOnly,
        std::string("../Sound/") + filenameOnly,
        std::string("2048/Sound/") + filenameOnly,
        exeDir + "\\Musik\\" + filenameOnly,
        std::string("Musik/") + filenameOnly,
        std::string("CODE/Musik/") + filenameOnly,
        exeDir + "\\CODE\\Musik\\" + filenameOnly,
        exeDir + "\\..\\Musik\\" + filenameOnly,
        std::string("../Musik/") + filenameOnly,
        std::string("2048/Musik/") + filenameOnly,
        std::string("./") + filenameOnly
    };

    for (size_t i = 0; i < sizeof(candidates)/sizeof(candidates[0]); i++) {
        std::ifstream test(candidates[i].c_str(), std::ios::binary);
        if (test.good()) {
            char full[MAX_PATH];
            if (GetFullPathNameA(candidates[i].c_str(), MAX_PATH, full, NULL) != 0) {
                return std::string(full);
            }
            return candidates[i];
        }
    }
    return std::string(filepath);
#else
    return std::string(filepath ? filepath : "");
#endif
}

inline void stopAudio() {
#ifdef _WIN32
    PlaySoundA(NULL, NULL, 0);
#endif
}

inline void playAudio(const char* filepath) {
#ifdef _WIN32
    if (!filepath || strlen(filepath) == 0) return;

    // 1. Hentikan audio sebelumnya
    stopAudio();

    // 2. Dapatkan path absolut file
    std::string fullpath = resolveAudioPath(filepath);

    // 3. Putar via PlaySoundA (Native WinMM, 100% stabil untuk aplikasi konsol Windows tanpa dependensi DirectShow)
    PlaySoundA(fullpath.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
#else
    (void)filepath;
#endif
}

// SFX ketikan typewriter menggunakan MCI waveaudio terpisah agar tidak menghentikan BGM PlaySoundA
inline void initTypewriterSFX() {
#ifdef _WIN32
    std::string clickPath = resolveAudioPath("Musik/typeclick.wav");
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "open \"%s\" type waveaudio alias typeclick_sfx", clickPath.c_str());
    mciSendStringA(cmd, NULL, 0, NULL);
#endif
}

inline void playTypewriterSFX() {
#ifdef _WIN32
    mciSendStringA("play typeclick_sfx from 0", NULL, 0, NULL);
#endif
}

inline void closeTypewriterSFX() {
#ifdef _WIN32
    mciSendStringA("close typeclick_sfx", NULL, 0, NULL);
#endif
}

#endif