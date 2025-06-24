// render.c
#include "render.h"
#include <stdio.h>

void app_render(void *appstate) {
    AppState* state = (AppState*) appstate;

    // WICHTIG: Setze die Hintergrundfarbe VOR RenderClear
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);

    // Rendere alle Entities
    for (int i = 0; i < entities_count; i++) {
        if (!entities[i].render) continue;
        entities[i].render(state->renderer, entities[i].data);
    }

    SDL_RenderPresent(state->renderer);
}