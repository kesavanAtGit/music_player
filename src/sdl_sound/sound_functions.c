#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_sound.h>

int main(int argc, char** argv) {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Sound_Init();

    const Sound_DecoderInfo** info;
    info = Sound_AvailableDecoders();

    // returns NULL after the call to a current error
    Sound_ClearError();
    if(info) {
        fprintf(stderr, "%s\n", Sound_GetError());
    }

    Sound_Version* compiled;
    compiled = malloc(sizeof(Sound_Version));
    SOUND_VERSION(compiled);
    printf("%d\n%d\n%d\n", compiled->major, compiled->minor, compiled->patch);

    Sound_Version* linked;
    linked = malloc(sizeof(Sound_Version));
    Sound_GetLinkedVersion(linked);
    printf("%d\n%d\n%d\n", linked->major, linked->minor, linked->patch);

    Sound_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
    return 0;
}
