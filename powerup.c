// powerup.c
#include "powerup.h"
#include <SDL3_image/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "player.h"
// Globale Variablen
static Powerup powerups[MAX_POWERUPS];
static SDL_Texture* powerup_texture = NULL;
static float double_shoot_timer = 0.0f;
static float speed_timer=0.0f;
static bool double_shoot_active = false;
static bool speed_active=false;
static int bombs=0;
static int powerups_spawned_this_wave = 0;
static int current_wave = 0;


// Power-Up Definitionen


static const PowerupDefinition powerup_definitions[POWERUP_TYPE_COUNT] = {
    [POWERUP_DOUBLE_SHOOT] = {
        .type = POWERUP_DOUBLE_SHOOT,
        .sprite_rect = {25, 41, 6, 5},
        .duration = 5.0f,
        .effect_strength = 4.0f,
        .fall_speed = 30.0f,
        .spawn_chance =0.0f
    },
    [POWERUP_HEART]={
        .type = POWERUP_HEART,
        .sprite_rect = {32,66,8,6},
        .duration = 0,
        .effect_strength = 1.0f,
        .fall_speed = 35.0f,
        .spawn_chance = 00.0f
    },
    [POWERUP_SPEED]={
        .type = POWERUP_SPEED,
        .sprite_rect = {25,57,5,6},
        .duration = 8.0f,
        .effect_strength = 1.8f,
        .fall_speed = 30.0f,
        .spawn_chance = 0.0f
    },
    [POWERUP_BOMB]={
        .type = POWERUP_BOMB,
        .sprite_rect = {34,50,4,5},
        .duration = 0.0f,
        .effect_strength = 1.0f,
        .fall_speed = 30.0f,
        .spawn_chance = 100.0f
    }
};


// Initialisierung

static void init_powerups(SDL_Renderer* renderer) {
    // Alle Power-Ups zuruecksetzen
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = false;
    }

    // Textur laden (gleiche wie Enemies)
    if (!powerup_texture) {
        const char path[] = "C:\\Users\\tb\\CLionProjects\\SpaceinvaderRichtig1\\pictures\\pico8_invaders_sprites_LARGE.png";
        powerup_texture = IMG_LoadTexture(renderer, path);
        if (!powerup_texture) {
            printf("Failed to load powerup texture: %s\n", SDL_GetError());
        } else {
            printf("Powerup texture loaded successfully\n");
        }
    }
}

// Power-Up spawnen
static PowerupType get_random_powerup_type() {
    float total_chance = 0;
    for (int i = 0; i < POWERUP_TYPE_COUNT; i++) {
        total_chance += powerup_definitions[i].spawn_chance;
    }

    float random_value = (float)(rand() % (int)total_chance);
    float accumulated = 0;

    for (int i = 0; i < POWERUP_TYPE_COUNT; i++) {
        accumulated += powerup_definitions[i].spawn_chance;
        if (random_value < accumulated) {
            return (PowerupType)i;
        }
    }

    return POWERUP_DOUBLE_SHOOT;  // Fallback
}
void powerup_spawn(float x, float y) {
    PowerupType type=get_random_powerup_type();
    powerup_spawn_specific(x, y, type);
}
void powerup_spawn_specific(float x, float y, PowerupType type) {
    if (type >= POWERUP_TYPE_COUNT) {
        printf("Falsches powerup type: %d\n", type);
        return;
    }
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) {
            const PowerupDefinition* def = &powerup_definitions[type];

            powerups[i].type = def->type;
            powerups[i].sprite_rect = def->sprite_rect;
            powerups[i].position.x = x;
            powerups[i].position.y = y;
            powerups[i].position.w = def->sprite_rect.w * 4;  // 4x Skalierung
            powerups[i].position.h = def->sprite_rect.h * 4;
            powerups[i].fall_speed = def->fall_speed;
            powerups[i].duration = def->duration;
            powerups[i].effect_strength = def->effect_strength;
            powerups[i].active = true;

            printf("Double Shoot Power-Up spawned at %.2f, %.2f\n", x, y);
            return;
        }
    }
}


// Update
static void update(float delta_time, void* data) {
    // Update Double Shoot Timer
    if (double_shoot_active) {
        double_shoot_timer -= delta_time;
        if (double_shoot_timer <= 0) {
            double_shoot_active = false;
            printf("Double Shoot expired\n");
        }
    }
    if (speed_active) {
        speed_timer -= delta_time;
        if (speed_timer <= 0) {
            speed_active = false;
            printf("Double Shoot expired\n");
        }
    }

    // Update alle Power-Ups
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            // Fallen lassen
            powerups[i].position.y += powerups[i].fall_speed * delta_time;

            // Entfernen wenn ausserhalb des Bildschirms
            if (powerups[i].position.y > 650) {
                powerups[i].active = false;
            }
        }
    }
}

// Render
static void render(SDL_Renderer* renderer, void* data) {
    if (!powerup_texture) return;

    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            SDL_FRect src = {
                powerups[i].sprite_rect.x,
                powerups[i].sprite_rect.y,
                powerups[i].sprite_rect.w,
                powerups[i].sprite_rect.h
            };

            SDL_SetTextureScaleMode(powerup_texture, SDL_SCALEMODE_NEAREST);
            SDL_RenderTexture(renderer, powerup_texture, &src, &powerups[i].position);
        }
    }

    // Debug: Zeige Double Shoot Status
    if (double_shoot_active) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        char timer_text[64];
        snprintf(timer_text, sizeof(timer_text), "DOUBLE SHOOT: %.1fs", double_shoot_timer);
        SDL_RenderDebugText(renderer, 10, 80, timer_text);
    }
    if (speed_active) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        char timer_text[64];
        snprintf(timer_text, sizeof(timer_text), "Speed: %.1fs", speed_timer);
        SDL_RenderDebugText(renderer, 10, 80, timer_text);
    }
    if (bombs>=1) {
        SDL_FRect bomb_sprite = {34, 50, 4, 5};

        for (int i = 0; i < bombs; i++) {
            SDL_FRect bomb_position = {
                700.0f - i * 25.0f,  // X-Position mit Abstand
                10.0f,               // Y-Position
                16.0f,               // Breite (4 * 4)
                20.0f                // Höhe (5 * 4)
            };

            SDL_SetTextureScaleMode(powerup_texture, SDL_SCALEMODE_NEAREST);
            SDL_RenderTexture(renderer, powerup_texture, &bomb_sprite, &bomb_position);
        }
    }

}

// Event Handler
static void handle_events(SDL_Event* event, void* data) {
    // Keine speziellen Events
}

// Cleanup
static void cleanup(void* data) {
    if (powerup_texture) {
        SDL_DestroyTexture(powerup_texture);
        powerup_texture = NULL;
    }
}

// Kollisionspruefung
Powerup* powerup_check_collision(SDL_FRect* player_rect) {
    if (!player_rect) return NULL;

    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            // Einfache AABB Kollision
            if (powerups[i].position.x < player_rect->x + player_rect->w &&
                powerups[i].position.x + powerups[i].position.w > player_rect->x &&
                powerups[i].position.y < player_rect->y + player_rect->h &&
                powerups[i].position.y + powerups[i].position.h > player_rect->y) {

                // Aktiviere Effekt
                switch(powerups[i].type){
                    case POWERUP_DOUBLE_SHOOT:
                        double_shoot_active = true;
                        double_shoot_timer = powerups[i].duration;
                        printf("Double Shoot activated for %.1f seconds!\n", powerups[i].duration);
                    break;
                    case POWERUP_HEART:
                        get_bonus(powerups[i].effect_strength);
                    break;
                    case POWERUP_SPEED:
                        speed_active=true;
                        speed_timer = powerups[i].duration;
                    printf("speed activated for %.1f seconds!\n", powerups[i].duration);
                    break;
                    case POWERUP_BOMB:
                        bombs++;
                    break;
                }

                // Deaktiviere Power-Up
                powerups[i].active = false;
                return &powerups[i];
            }
        }
    }
    return NULL;
}

void bomb_got_used() {
    if (bombs > 0) {
        bombs--;
        printf("Bombe verwendet! Verbleibend: %d\n", bombs);
    }
}

bool is_speed_active(void) {
    return speed_active;
}
float get_speed_multplier(void) {
    if (speed_active) {
        return 1.0f * powerup_definitions[POWERUP_SPEED].effect_strength;
    }
    else {
        return 1.0f;
    }
}
// Double Shoot Status
bool is_double_shoot_active(void) {
    return double_shoot_active;
}
int get_bomb_count() {
    return bombs;
}
float get_double_shoot_multiplier(void) {
    if (double_shoot_active) {
        return 1.0f / powerup_definitions[POWERUP_DOUBLE_SHOOT].effect_strength;
    }
    else {
        return 1.0f;
    }
}

// Wave Callbacks
void powerup_on_wave_start(int wave_number) {
    current_wave = wave_number;
    powerups_spawned_this_wave = 0;

    // Alle Power-Ups zuruecksetzen
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = false;
    }

    printf("Power-Up System: Wave %d started\n", wave_number);
}

void powerup_on_enemy_killed(float x, float y) {
    // Nur ab Welle 2
    if (current_wave < 1) return;

    // Max 2 Power-Ups pro Welle
    if (powerups_spawned_this_wave >= 2) return;

    // Zufaellige Chance (10% pro getoeteten Gegner)
    int chance = 100; //rand() % 100;
    if (chance < 1000) {
        powerup_spawn(x, y);
        powerups_spawned_this_wave++;
        printf("Power-Up %d/%d spawned this wave\n", powerups_spawned_this_wave, 2);
    }
}

// Alle Power-Ups zuruecksetzen
void powerup_reset_all(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = false;
    }
    double_shoot_active = false;
    double_shoot_timer = 0.0f;
    speed_active=false;
    speed_timer=0.0f;
    powerups_spawned_this_wave = 0;
}

// Entity erstellen
Entity init_powerup_system(SDL_Renderer* renderer) {
    printf("Initialisiere Power-Up System...\n");

    init_powerups(renderer);

    Entity powerup_entity = {
        .update = update,
        .render = render,
        .handle_events = handle_events,
        .cleanup = cleanup,
        .data = NULL
    };

    return powerup_entity;
}