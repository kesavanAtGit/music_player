#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(SDL_INIT_AUDIO);

    if(Mix_OpenAudioDevice(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4092, NULL, 0) < 0) {
        fprintf(stderr, "Error in opening the device: %s\n", Mix_GetError());
        Mix_CloseAudio();
        Mix_Quit();
        exit(-1);
    }

    int i = 1;
    while(i < argc) {
        Mix_Music* audio_music = Mix_LoadMUS(argv[i]);

        if(!audio_music) {
            fprintf(stderr, "Error in loading the music: %s\n", Mix_GetError());
            Mix_FreeMusic(audio_music);
            Mix_CloseAudio();
            Mix_Quit();
            exit(-1);
        }

        int channel = Mix_PlayMusic(audio_music, 1);

        if(channel == -1) {
            fprintf(stderr, "Error in playing the music: %s\n", Mix_GetError());
            Mix_FreeMusic(audio_music);
            Mix_CloseAudio();
            Mix_Quit();
            exit(-1);
        }

        while(Mix_PlayingMusic() != 0) {
            SDL_Delay(1000);
        }

        Mix_FreeMusic(audio_music);
        i = i + 1;
    }
    Mix_CloseAudio();

    Mix_Quit();
    return 0;
}
