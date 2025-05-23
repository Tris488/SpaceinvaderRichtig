//
// Created by tb on 09.04.2025.
//

#ifndef PLAYER_H
#define PLAYER_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"
#include "pew.h"
#include <SDL3/SDL_joystick.h>
#include "mainfolder/wichitg.h"
typedef struct {
    float x,y;
}Position;
Entity init_player(SDL_Renderer* renderer);
#endif //PLAYER_H
