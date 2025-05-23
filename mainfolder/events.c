#include "wichitg.h"
#include <stdio.h>
#include <stdlib.h>

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    // Basic joystick event logging
    if (event->type == SDL_EVENT_JOYSTICK_AXIS_MOTION) {
        if (abs(event->jaxis.value) > 8000) { // Only log significant movements
            printf("Joystick axis movement: Axis=%d, Value=%d\n",
                  event->jaxis.axis, event->jaxis.value);
        }
    }

    if (event->type == SDL_EVENT_JOYSTICK_ADDED) {
        printf("Joystick connected\n");
    }

    if (event->type == SDL_EVENT_JOYSTICK_REMOVED) {
        printf("Joystick disconnected\n");
    }

    if (event->type == SDL_EVENT_JOYSTICK_BUTTON_DOWN) {
        printf("Joystick button pressed: %d\n", event->jbutton.button);
    }

    if (event->type == SDL_EVENT_JOYSTICK_BUTTON_UP) {
        printf("Joystick button released: %d\n", event->jbutton.button);
    }

    // Pass the event to all entities that have a handle_events function
    for (int i = 0; i < entities_count; i++) {
        if (entities[i].handle_events) {
            entities[i].handle_events(event, entities[i].data);
        }
    }

    return SDL_APP_CONTINUE;
}