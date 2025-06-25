// powerup.h
#ifndef POWERUP_H
#define POWERUP_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "entity.h"

#define MAX_POWERUPS 10

// Power-Up Typen
typedef enum {
    POWERUP_DOUBLE_SHOOT,
    POWERUP_TYPE_COUNT
} PowerupType;

// Power-Up Struktur
typedef struct {
    PowerupType type;
    SDL_FRect position;
    SDL_FRect sprite_rect;
    float fall_speed;
    bool active;

    // Effekt-Eigenschaften
    float duration;
    float effect_strength;
} Powerup;

// Power-Up System initialisieren
Entity init_powerup_system(SDL_Renderer* renderer);

// Power-Up spawnen
void powerup_spawn(float x, float y);

// Kollision pruefen
Powerup* powerup_check_collision(SDL_FRect* player_rect);

// Alle Power-Ups zuruecksetzen
void powerup_reset_all(void);

// Double Shoot Status
bool is_double_shoot_active(void);
float get_double_shoot_multiplier(void);

// Wave-Callback fuer Power-Up Spawning
void powerup_on_wave_start(int wave_number);
void powerup_on_enemy_killed(float x, float y);

#endif // POWERUP_H