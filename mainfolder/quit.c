//
// Created by tb on 03.04.2025.
//

#include "quit.h"
#include "../database.h"
#include "../joystick_manager.h"


SDL_AppResult SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState *state = (AppState *)appstate;

    // Joystick-Manager aufräumen
    joystick_manager_cleanup();
    SDL_Log("Joystick Manager cleaned up");

    // Andere Cleanup-Aufgaben...
    if (state) {
        if (state->renderer) {
            SDL_DestroyRenderer(state->renderer);
        }
        if (state->window) {
            SDL_DestroyWindow(state->window);
        }
        SDL_free(state);
    }

    SDL_Quit();
    return result;
}