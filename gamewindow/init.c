#include "init.h"
#include <stdio.h>

#include "../health.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    AppState *state = SDL_malloc(sizeof(AppState));
    *appstate = state;
    int height=600;
    int width=800;

    // Initialize SDL with proper flags - fix the logical operator
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    } else {
        SDL_Log("SDL initialized successfully");
    }

    // Enable joystick events
    SDL_SetJoystickEventsEnabled;

    state->window = SDL_CreateWindow("Space Invaders", width, height, NULL);

    if (!state->window) {
        SDL_Log("Error creating window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->renderer = SDL_CreateRenderer(state->window, NULL);
    if (!state->renderer) {
        SDL_Log("Error creating renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize game entities
    entities[entities_count++] = init_map(state->renderer);
    printf("entities_count before init_player: %i\n", entities_count);
    entities[entities_count++] = init_player(state->renderer);
    printf("Player initialized, entity count: %i\n", entities_count);
    entities[entities_count++]=init_health(state->renderer);
    printf("health initialized, entity count: %i\n", entities_count);


    return SDL_APP_CONTINUE;
}
