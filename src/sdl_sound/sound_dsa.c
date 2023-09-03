#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_sound.h>

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_AUDIO);
    Sound_Init();

    // basics of a decoded sample's data structure
    Sound_AudioInfo* info;
    info->rate = 44100;
    info->format = AUDIO_S16SYS;
    info->channels = 2;

    const Sound_DecoderInfo** ext;
    for(ext = Sound_AvailableDecoders(); *ext != NULL; ++ext) {
        printf("%s\n %s\n %s\n %s\n", *(*ext)->extensions, (*ext)->description, (*ext)->author, (*ext)->url);
        printf("\n");
    }

    Sound_Version version;
    SOUND_VERSION(&version);

    SDL_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    return 0;
}
