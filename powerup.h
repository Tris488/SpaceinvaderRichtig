// powerup.h
#ifndef POWERUP_H
#define POWERUP_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "entity.h"
#include "health.h"

#define MAX_POWERUPS 10

// Power-Up Typen
typedef enum {
    POWERUP_DOUBLE_SHOOT,
    POWERUP_HEART,
    POWERUP_SPEED,
    POWERUP_BOMB,
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
typedef struct {
    PowerupType type;
    SDL_FRect sprite_rect;
    float duration;
    float effect_strength;
    float fall_speed;
    float spawn_chance;
} PowerupDefinition;


Entity init_powerup_system(SDL_Renderer* renderer);
void powerup_spawn(float x, float y);
void powerup_spawn_specific(float x, float y, PowerupType type);
Powerup* powerup_check_collision(SDL_FRect* player_rect);
void powerup_reset_all(void);
bool is_double_shoot_active(void);
float get_double_shoot_multiplier(void);
void powerup_on_wave_start(int wave_number);
void powerup_on_enemy_killed(float x, float y);
float get_speed_multplier(void);
bool is_speed_active();
void bomb_got_used();
int get_bomb_count();




#endif // POWERUP_H