#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

void initializeSDL() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }
}

void cleanupSDL() {
    Mix_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initializeSDL();

    Mix_Music *music = Mix_LoadMUS("sound.mp3");
    if (music == NULL) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        cleanupSDL();
        return 1;
    }

    Mix_PlayMusic(music, -1); // Play music indefinitely

    printf("Press Enter to stop the music...\n");
    getchar();

    Mix_FreeMusic(music);
    cleanupSDL();

    return 0;
}
