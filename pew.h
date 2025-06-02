#ifndef SHOT_H
#define SHOT_H

#include "entity.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>  // Für bool-Typ

// Verbesserte ShotData-Struktur
typedef struct {
    float x;          // X-Position
    float y;          // Y-Position
    float speed;      // Geschwindigkeit
    Uint64 created;   // Erstellungszeitpunkt
    Uint64 lasttick;
    float time;
    int id;           // Schuss-ID
    bool active;      // Ist der Schuss aktiv?
    bool should_remove; // Soll der Schuss entfernt werden?
} ShotData;

Entity create_shot_entity(SDL_Renderer* renderer, float x, float y);

// Globale Aufräumfunktion
void cleanup_inactive_shots();

#endif