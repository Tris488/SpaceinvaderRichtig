// enemy_entity.h
#ifndef ENEMY_ENTITY_H
#define ENEMY_ENTITY_H

#include <SDL3/SDL.h>
#include "entity.h"

// Initialisiere das Enemy-System als Entity
Entity init_enemy_system(SDL_Renderer* renderer);

#endif // ENEMY_ENTITY_H