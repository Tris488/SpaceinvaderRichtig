//
// Created by tb on 19.05.2025.
//
/**
#include "button.h"
#include "entity.h"
static void update(float delta_time, void* state) {
    if (!state) return;


    // Suche nach allen Schüssen im Entitäten-Array und aktualisiere sie
    int removed_entities = 0;

    for (int i = 0; i < entities_count; i++) {
        // Überspringe Nicht-Schuss-Entitäten oder inaktive Entitäten
        if (!entities[i].data || entities[i].update != update) continue;

        Buttondata* button = (Buttondata*)entities[i].data;

        }

}

// Render-Funktion für Schüsse
void render(SDL_Renderer* renderer, void* data) {
    Buttondata* button =(Buttondata*) data;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  /* blue, full alpha

    SDL_RenderFillRect(renderer, &button->rect);
    //Text
    SDL_RenderDebugText(renderer, button->rect.h, button->rect.w, "You can do it in different colors.");
        // Center the text on the button


        };


// Aufräumfunktion
static void cleanup(void* data){
    if (data) {
        free(data);
    }
}

// Keine spezifischen Events für Schüsse
static void handle_events(SDL_Event* event, void* data) {

}

// Funktion zur Erstellung eines neuen Schusses
Entity create_button_entity(SDL_Renderer* renderer) {
    Buttondata* button = (Buttondata*)malloc(sizeof(Buttondata));

        button->rect.x = 100;
        button->rect.y = 100;
        button->rect.w = BUTTON_WIDTH;
        button->rect.h = BUTTON_HEIGHT;
        button->text = "Halloo";
        button->backgroundColor = (SDL_Color){100, 100, 100, 255}; // Grey
        button->hoverColor = (SDL_Color){150, 150, 150, 255}; // Light grey
        button->textColor = (SDL_Color){255, 255, 255, 255}; // White
        button->isHovered = true;;


    // Entity erstellen
    Entity ebutton = {
        .update = update,
        .render = render,
        .handle_events = handle_events,
        .cleanup = cleanup,
        .data = button,
    };

    return ebutton;
}
*/