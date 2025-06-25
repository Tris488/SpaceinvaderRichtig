// enemy_entity.c
#include "enemy.h"
#include "entity.h"
#include "mainfolder/wichitg.h"
#include "gameover.h"
#include <stdio.h>
#include <stdlib.h>
#include "powerup.h"
#include "enemy.h"
// Entity-Wrapper für das Enemy-System
typedef struct {
    int current_wave;
    bool wave_started;
} EnemySystemData;

static EnemySystemData* enemy_system_data = NULL;

// Update-Funktion für das Enemy-System
static void enemy_system_update(float delta_time, void* data) {
    if (!enemy_system_data) return;

    // Keine Updates bei Game Over
    if (is_game_over()) {
        // Reset wave_started wenn Game Over, damit beim Neustart die Welle startet
        enemy_system_data->wave_started = false;
        return;
    }

    // Starte erste Welle, falls noch nicht geschehen
    if (!enemy_system_data->wave_started) {
        wave_init(1);
        powerup_on_wave_start(1);
        enemy_system_data->wave_started = true;
        enemy_system_data->current_wave = 1;
        printf("Erste Welle gestartet!\n");
    }

    // Update Wellen-System
    wave_update(delta_time);

    // Update alle Gegner
    enemy_update_all(delta_time);

    // Prüfe ob Welle abgeschlossen
    if (wave_is_complete()) {
        printf("Welle %d abgeschlossen! Score: %d\n",
               wave_get_current(), score_get_current());

        // Kleine Pause bevor nächste Welle startet
        static float wave_delay = 0;
        wave_delay += delta_time;

        if (wave_delay > 2.0f) { // 2 Sekunden Pause
            wave_start_next();
            powerup_on_wave_start(wave_get_current());
            enemy_system_data->current_wave = wave_get_current();
            wave_delay = 0;
            printf("Nächste Welle %d startet!\n", enemy_system_data->current_wave);
        }
    }
}

// Render-Funktion für das Enemy-System
static void enemy_system_render(SDL_Renderer* renderer, void* data) {
    // Rendere alle Gegner
    enemy_render_all(renderer);

    // Debug-Info: Score anzeigen
    // (In einem echten Spiel würdest du hier eine richtige UI verwenden)
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score: %d Wave: %d",
             score_get_current(), wave_get_current());

    // SDL3 hat SDL_RenderDebugText
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 150, 10, score_text);
}

// Event-Handler
static void enemy_system_handle_events(SDL_Event* event, void* data) {
    // Keine speziellen Events für das Enemy-System
}

// Cleanup
static void enemy_system_cleanup(void* data) {
    enemy_cleanup_system();

    if (enemy_system_data) {
        free(enemy_system_data);
        enemy_system_data = NULL;
    }
}

// Initialisierung des Enemy-Systems als Entity
Entity init_enemy_system(SDL_Renderer* renderer) {
    printf("Initialisiere Enemy-System\n");

    // Initialisiere das Enemy-System
    enemy_init_system(renderer);

    // Erstelle System-Daten
    enemy_system_data = (EnemySystemData*)malloc(sizeof(EnemySystemData));
    if (enemy_system_data) {
        enemy_system_data->current_wave = 0;
        enemy_system_data->wave_started = false;
    }

    Entity enemy_system = {
        .update = enemy_system_update,
        .render = enemy_system_render,
        .handle_events = enemy_system_handle_events,
        .cleanup = enemy_system_cleanup,
        .data = enemy_system_data
    };

    return enemy_system;
}