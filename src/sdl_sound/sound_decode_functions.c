#include <SDL2/SDL_rwops.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_sound.h>

int main(int argc, char** argv) {
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    Sound_Init();

    // Sound_Decode(&sample);
    // Sound_DecodeAll(); Dynamically allocate memory for the rest of the sample data

    Sound_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
    return 0;
}
