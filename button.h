//
// Created by tb on 19.05.2025.
//
/**
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"

#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 80
Entity create_button_entity(SDL_Renderer* renderer);
typedef struct {
    SDL_Rect rect;
    const char* text;
    SDL_Color backgroundColor;
    SDL_Color hoverColor;
    SDL_Color textColor;
    bool isHovered;
}Buttondata;
#endif //BUTTON_H
*/