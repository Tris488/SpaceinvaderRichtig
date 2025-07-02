//
// Created by tb on 08.05.2025.
//

#include "health.h"
#include "gameover.h"
#include <stdio.h>
#include "enemy.h"

static SDL_Texture* health_texture = NULL;
int healthpoints = 3;

typedef struct {
    float x, y;
} HealthData;

static void cleanup() {

}

static void handle_events(SDL_Event* event, void* data){

}

static void update(float delta_time, void* data) {
    // Nichts zu updaten - Leben werden direkt gerendert
}

static void render(SDL_Renderer* renderer, void* data) {
    if (!health_texture) {
        printf("WARNING: Health texture ist NULL!\n");
        return;
    }

    // Sprite-Position des Herzens im Spritesheet
    SDL_FRect heart_sprite = {32, 57, 8, 7}; // 40-32=8, 64-57=7

    // Rendere so viele Herzen wie Leben vorhanden
    for (int i = 0; i < healthpoints; i++) {
        // Position auf dem Bildschirm (3x skaliert)
        SDL_FRect heart_position = {
            10 + i * 35,  // X-Position mit Abstand
            10,           // Y-Position
            24,           // Breite (8 * 3)
            21            // Höhe (7 * 3)
        };

        SDL_SetTextureScaleMode(health_texture, SDL_SCALEMODE_NEAREST);
        SDL_RenderTexture(renderer, health_texture, &heart_sprite, &heart_position);
    }

    // Optional: Debug-Text
    char health_text[32];
    snprintf(health_text, sizeof(health_text), "Leben: %d", healthpoints);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 10, 40, health_text);
}

// Spieler nimmt Schaden
void damage_player(int damage) {
    healthpoints -= damage;
    if (healthpoints < 0) {
        healthpoints = 0;
    }

    printf("Spieler nimmt %d Schaden! Verbleibende Leben: %d\n", damage, healthpoints);

    if (healthpoints == 0) {
        printf("GAME OVER!\n");
        set_game_over(true); // Aktiviere Game Over Screen
    }
}

// Leben zurueckgeben
int get_player_health(void) {
    return healthpoints;
}
void get_bonus(int value) {
    healthpoints+=value;
}

// Leben setzen (fuer Reset)
void set_player_health(int health) {
    healthpoints = health;
    if (healthpoints > 3) healthpoints = 3;
    if (healthpoints < 0) healthpoints = 0;
    printf("Spieler Leben gesetzt auf: %d\n", healthpoints);
}

Entity init_health(SDL_Renderer* renderer) {
    printf("Initialisiere Health-System...\n");

    // Verwende das gleiche Spritesheet wie Player und Enemies;
    health_texture = IMG_LoadTexture(renderer, "pictures\\pico8_invaders_sprites_LARGE.png");

    if (!health_texture) {
        printf("FEHLER: Konnte pico8_invaders_sprites_LARGE.png nicht laden!\n");
        printf("SDL_GetError: %s\n", SDL_GetError());
    } else {
        printf("Health-System verwendet pico8_invaders_sprites_LARGE.png\n");
    }

    // Setze initiale Werte
    healthpoints = 3;

    Entity health = {
        .cleanup = cleanup,
        .handle_events = handle_events,
        .update = update,
        .render = render,
        .data = NULL
    };
    return health;
}