#include <SDL2/SDL_rwops.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_sound.h>

int main(int argc, char** argv) {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Sound_Init();

    SDL_RWops* rw = SDL_RWFromFile("../music_files/flanging-square-10-seconds-38605.mp3", "r");

    Uint32* buf;
    Sound_Sample* sample = Sound_NewSample(rw, NULL, NULL, sizeof(buf));

    if(sample != NULL) {
        printf("%d\n", sample->buffer_size);
    }

    // Sound_NewSampleFromFile();
    // Sound_Seek();
    // Sound_SetBufferSize();

    Sound_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
    return 0;
}
