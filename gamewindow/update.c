//
// Created by tb on 03.04.2025.
//

#include "update.h"

void app_update(void *appstate) {
    AppState* state = (AppState*) appstate;
       printf("delta_time: %.4f\n", state->delta_time); // Hinzugefügt
    state->last_tick = state->current_tick;
    state->current_tick = SDL_GetTicks();
    state->delta_time = (state->current_tick - state->last_tick) / 1000.0f;

    for (int i = 0; i < entities_count; i++) {
        if (!entities[i].update) continue;
        entities[i].update(state->delta_time, state);
    }
}
