// game_scale.h
#ifndef GAME_SCALE_H
#define GAME_SCALE_H

#include <SDL3/SDL.h>

// Basis-Auflösung (Original-Design)
#define BASE_WIDTH 800
#define BASE_HEIGHT 600

// Globale Skalierungsvariablen
extern float game_scale;
extern int game_offset_x;
extern int game_offset_y;
extern int scaled_width;
extern int scaled_height;

// Funktionen
void update_game_scale(SDL_Renderer* renderer);
float scale_value(float value);
int scale_x(int x);
int scale_y(int y);
SDL_FRect scale_rect(SDL_FRect rect);

#endif // GAME_SCALE_H