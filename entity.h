//
// Created by tb on 07.04.2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <SDL3/SDL.h>
#include <stdio.h>
#define MAX_ENTITIES 100000

typedef struct Entity {
    void (*update)(float delta_time, void* data);
    void (*render)(SDL_Renderer*, void* data);
    void (*handle_events)(SDL_Event* event, void* data);
    void (*cleanup)(void* data);
    void* data;
} Entity;


extern Entity entities[MAX_ENTITIES];
extern int entities_count;
#endif //ENTITY_H
