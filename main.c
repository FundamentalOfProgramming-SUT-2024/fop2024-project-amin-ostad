#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <ncurses.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include "login.h"
#include "map.h"

#define NUM_OPTIONS 4
void cleanupSDL() {
    Mix_Quit();
    SDL_Quit();
}

void initializeSDL() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        exit(1);
    }
}

void play_music(){
    initializeSDL();

    Mix_Music *music = Mix_LoadMUS("sound.mp3");
    if (music == NULL) {
        cleanupSDL();
    }

    Mix_PlayMusic(music, -1);

    first_page();

    Mix_FreeMusic(music);
    cleanupSDL();
}

int main() {
    play_music();
}
