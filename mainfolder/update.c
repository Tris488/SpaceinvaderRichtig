//
// Created by tb on 03.04.2025.
//

#include "update.h"
#include "../pew.h"  // Für cleanup_inactive_shots

void app_update(void *appstate) {
    if (!appstate) return;

    AppState* state = (AppState*) appstate;
    state->last_tick = state->current_tick;
    state->current_tick = SDL_GetTicks();
    state->delta_time = (state->current_tick - state->last_tick) / 1000.0f;

    // Begrenze delta_time um Sprünge zu vermeiden
    if (state->delta_time > 0.1f) {
        state->delta_time = 0.1f;
    }

    // Update alle Entities
    for (int i = 0; i < entities_count; i++) {
        if (!entities[i].update) continue;

        // Player entities bekommen den AppState
        if (entities[i].data == NULL) {
            // Dies ist wahrscheinlich der Player
            entities[i].update(state->delta_time, state);
        } else {
            // Andere Entities bekommen ihre eigenen Daten
            entities[i].update(state->delta_time, entities[i].data);
        }
    }

    // Aufräumen inaktiver Schüsse
    cleanup_inactive_shots();
}