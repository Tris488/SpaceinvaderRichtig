//
// Created by tb on 08.05.2025.
//

#ifndef HEALTH_H
#define HEALTH_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"
#include <SDL3/SDL_joystick.h>

Entity init_health(SDL_Renderer* renderer);

// Neue Funktionen für das Health-System
extern int healthpoints;
void damage_player(int damage);
int get_player_health(void);
void set_player_health(int health);
void get_bonus(int value);

#endif //HEALTH_H