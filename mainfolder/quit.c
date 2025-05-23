//
// Created by tb on 03.04.2025.
//

#include "quit.h"

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState *state = (AppState *) appstate;
    for (int i = 0; i < entities_count; i++) {
        if (entities[i].cleanup) {
            entities[i].cleanup(entities[i].data);
        }
    }
    SDL_DestroyRenderer(state->renderer);
    state->renderer = NULL;
    SDL_DestroyWindow(state->window);
    state->window = NULL;
    SDL_Quit();
    SDL_free(state);
}
