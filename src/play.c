#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_sound.h>
#include <SDL2/SDL_stdinc.h>
#include <stdio.h>
#include <xmmintrin.h>

#define MUS_PATH "/home/kesavan/Music/sample.wav"

Uint8 *buf;
Uint32 buf_len;

void audio_callback(void *userdata, Uint8 *stream, int len) {
    if (buf_len == 0) {
        SDL_memset(stream, 0, len);
        return;
    }
    len = (len < buf_len ? len : buf_len);
    SDL_memcpy(stream, buf, len);

    // moving the position of the buf here
    buf += len;
    buf_len -= len;
}

int main(int argc, char **argv) {
    int r = SDL_Init(SDL_INIT_AUDIO);
    if (r < 0) {
        fprintf(stderr, "SDL initialization error: %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_AudioSpec *desiredSpec, *obtainedSpec;
    desiredSpec = malloc(sizeof(SDL_AudioSpec));
    obtainedSpec = malloc(sizeof(SDL_AudioSpec));

    desiredSpec->freq = 44100;
    desiredSpec->format = AUDIO_S16LSB;
    desiredSpec->channels = 2;
    desiredSpec->samples = 4096;
    desiredSpec->callback = audio_callback;
    desiredSpec->userdata = NULL;

    Uint8 *audio_buf;
    Uint32 audio_len;

    if (SDL_LoadWAV(MUS_PATH, desiredSpec, &audio_buf, &audio_len) == NULL) {
        fprintf(stderr, "%s\n", SDL_GetError());
        exit(-1);
    }

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, desiredSpec, obtainedSpec, 0);
    if (audioDevice == 0) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        exit(-1);
    }
    free(desiredSpec);

    buf = audio_buf;
    buf_len = audio_len;

    // SDL_QueueAudio(audioDevice, audio_buf, audio_len);
    SDL_PauseAudioDevice(audioDevice, 0);

    while (buf_len > 0) {
        SDL_Delay(100);
    }

    printf("Press a key to quit \n");
    getchar();

    SDL_CloseAudioDevice(audioDevice);
    SDL_FreeWAV(audio_buf);
    SDL_Quit();

    return 0;
}
