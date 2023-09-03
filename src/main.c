#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

SDL_Window* create_window(const char* win_title, int pos_x, int pos_y, int width, int height, Uint32 flags) {
    static int SCREEN_WIDTH = 640;
    static int SCREEN_HEIGHT = 481;

    SDL_Window* win = SDL_CreateWindow(win_title, pos_x, pos_y, width, height, flags);

    if(win == NULL) {
        fprintf(stderr, "%s\n", SDL_GetError());
        exit(-1);
    }
    return win;
}

void fill_window_surface(SDL_Window* win) {
    SDL_Surface* win_surface = SDL_GetWindowSurface(win);

    if(win_surface == NULL) {
        fprintf(stderr, "%s\n", SDL_GetError());
        return;
    }

    if(SDL_FillRect(win_surface, NULL, SDL_MapRGB(win_surface->format, 0xFF, 0xFF, 0xFF)) < 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
        return;
    }

    if(SDL_UpdateWindowSurface(win) == 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
        return;
    }
}

int main(int argc, char** argv) {

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
    }

    SDL_Window* win = create_window("Hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    fill_window_surface(win);
    SDL_Delay(2000);

    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
