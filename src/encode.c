#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_sound.h>
#include <ctype.h>
#include <stdio.h>
#include <wctype.h>

#define MUS_PATH "/home/kesavan/Music/"

static Uint8 *buf;
static Uint32 buf_len;

void audio_callback
(void *userdata, Uint8 *stream, int len) {
    if (buf_len == 0) {
        SDL_memset(stream, 0, len);
        return;
    }
    len = (len < buf_len ? len : buf_len);
    SDL_memcpy(stream, buf, len);

    buf += len;
    buf_len -= len;
}

static const char*
Sound_GetFile(const char* file) {
    static char path[32];
    int i = 0;
    while(i < strlen(file) + 1) {
        path[i] = file[i];
        i = i + 1;
    }
    return path;
}

static const char*
Sound_GetFileFormat(const char* path) {
    int i = 0, index;
    while(i < strlen(path) + 1) {
        if(path[i] == '.') {
            index = i;
            break;
        }
        i = i + 1;
    }

    static char file_format[32];
    int j = 0;
    for(int k = index + 1; k < strlen(path) + 1; ++k) {
        file_format[j] = toupper(path[k]);
        j = j + 1;
    }
    return file_format;
}

int main(int argc, char **argv) {
    int init = SDL_Init(SDL_INIT_AUDIO);

    if(init < 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
        exit(-1);
    }

    Sound_Init();

    int i = 1;
    while(i < argc) {
        if(argc == 1) {
            fprintf(stderr, "Usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
            exit(-1);
        }else {
            const char* path = Sound_GetFile(argv[i]);
            SDL_RWops *rw = SDL_RWFromFile(path, "rb");

            const char* file_format = Sound_GetFileFormat(path);
            Sound_Sample *sample = Sound_NewSample(rw, file_format, NULL, 128);

            if (!sample) {
                fprintf(stderr, "%s\n", Sound_GetError());
                exit(-1);
            }

            int bytes_read = Sound_DecodeAll(sample);
            if (!bytes_read) {
                fprintf(stderr, "%s\n", Sound_GetError());
                exit(-1);
            }

            if (bytes_read != sample->buffer_size) {
                fprintf(stderr, "All bytes are not read \n");
                exit(-1);
            }

            SDL_AudioSpec *desiredSpec = malloc(sizeof(SDL_AudioSpec));
            desiredSpec->freq = sample->desired.rate;
            desiredSpec->format = sample->desired.format;
            desiredSpec->channels = sample->desired.channels;
            desiredSpec->callback = audio_callback;
            desiredSpec->userdata = NULL;

            int devid = SDL_OpenAudioDevice(NULL, 0, desiredSpec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
            if (devid == 0) {
                fprintf(stderr, "%s\n", Sound_GetError());
                exit(-1);
            }
            free(desiredSpec);

            buf = sample->buffer;
            buf_len = sample->buffer_size;

            SDL_PauseAudioDevice(devid, 0);
            while (buf_len > 0) {
                SDL_Delay(100);
            }

            Sound_FreeSample(sample);
            i = i + 1;
        }
    }
    Sound_Quit();

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
    return 0;
}

