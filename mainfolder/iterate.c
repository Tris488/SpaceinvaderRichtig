// iterate.c
#include "iterate.h"

SDL_AppResult SDL_AppIterate(void *appstate) {
    app_update(appstate); // Ruft die allgemeine Update-Funktion auf
    app_render(appstate);
    return SDL_APP_CONTINUE;
}