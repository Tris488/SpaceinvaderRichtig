#ifndef SHOT_H
#define SHOT_H

#include "entity.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

Entity create_shot_entity(SDL_Renderer* renderer, float x, float y);
// Verbesserte ShotData-Struktur
typedef struct {
    float x;          // X-Position
    float y;          // Y-Position
    float speed;      // Geschwindigkeit
    Uint64 created;   // Erstellungszeitpunkt
    int id;           // Schuss-ID
    bool active;      // Ist der Schuss aktiv?
} ShotData;
#endif
