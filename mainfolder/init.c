#include "init.h"
#include <stdio.h>
#include "../enemy_entity.h"
#include "../gameover.h"
#include "../powerup.h"

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

    state->window = SDL_CreateWindow("Space Invaders", width, height, 0);
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
    entities[entities_count++] = init_enemy_system(state->renderer);
    printf("Enemy system initialized, entity count: %i\n", entities_count);
    entities[entities_count++] = init_powerup_system(state->renderer);
    printf("Power-Up system initialized, entity count: %i\n", entities_count);
    entities[entities_count++] = init_player(state->renderer);
    printf("Player initialized, entity count: %i\n", entities_count);
    entities[entities_count++]=init_health(state->renderer);
    printf("health initialized, entity count: %i\n", entities_count);
    entities[entities_count++] = init_gameover_system(state->renderer);
    printf("Game Over system initialized, entity count: %i\n", entities_count);
    /*entities[entities_count++] = create_button_entity(state-> renderer);*/


    return SDL_APP_CONTINUE;
}