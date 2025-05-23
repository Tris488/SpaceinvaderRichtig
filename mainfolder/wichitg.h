//
// Created by tb on 03.04.2025.
//

#ifndef WICHITG_H
#define WICHITG_H
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "../Background.h"
#include "../entity.h"

typedef struct AppState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint64 last_tick;
    Uint64 current_tick;
    float delta_time;
} AppState;
#endif //WICHITG_H
