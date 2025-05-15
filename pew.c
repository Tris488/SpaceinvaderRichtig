// pew.c - Vollständig überarbeitete Version
#include "pew.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>  // Für bool-Typ

// Texture für Schüsse
static SDL_Texture* shot_texture = NULL;
static SDL_FRect spriteshot = {0, 4, 15, 19};



// Zähler für Schuss-IDs
static int next_shot_id = 1;

// Update-Funktion für Schüsse
static void update(float delta_time, void* state) {
    if (!state) return;


    // Suche nach allen Schüssen im Entitäten-Array und aktualisiere sie
    int removed_entities = 0;

    for (int i = 0; i < entities_count; i++) {
        // Überspringe Nicht-Schuss-Entitäten oder inaktive Entitäten
        if (!entities[i].data || entities[i].update != update) continue;

        ShotData* shot = (ShotData*)entities[i].data;

        // Ignoriere inaktive Schüsse
        if (!shot->active) continue;

        // Alte Position speichern für Debug-Ausgabe
        float old_y = shot->y;

        // Schuss-Bewegung
        shot->y -= shot->speed * delta_time;

        // Erzwinge Mindestbewegung
        if (shot->y == old_y) {
            shot->y -= 0.5f;
        }

        printf("Schuss #%d: y=%.2f → %.2f (Änderung: %.2f)\n",
               shot->id, old_y, shot->y, old_y - shot->y);

        // Prüfe, ob der Schuss den Bildschirm verlassen hat
        if (shot->y < 40) {
            printf("Schuss #%d verlässt Bildschirm, wird entfernt\n", shot->id);

            // Markiere diesen Schuss als inaktiv
            shot->active = false;

            // Aufräumen
            free(entities[i].data);
            entities[i].data = NULL;

            // Verschiebe die letzte Entität an diese Position
            if (i < entities_count - 1) {
                entities[i] = entities[entities_count - 1];
                i--; // Bleibe bei diesem Index für die nächste Iteration
            }

            // Reduziere die Anzahl der Entitäten
            entities_count--;
            removed_entities++;
        }
    }

    if (removed_entities > 0) {
        printf("%d Schüsse entfernt. Neue Entitäten-Anzahl: %d\n",
               removed_entities, entities_count);
    }
}

// Render-Funktion für Schüsse
static void render(SDL_Renderer* renderer, void* data) {
    if (!data || !renderer || !shot_texture) return;

    ShotData* shot = (ShotData*)data;

    // Nur aktive Schüsse rendern
    if (!shot->active) return;

    // Schuss-Rechteck
    SDL_FRect dst = {shot->x, shot->y, 10, 20};

    // Schuss rendern
    SDL_RenderTexture(renderer, shot_texture, &spriteshot, &dst);
}

// Aufräumfunktion
static void cleanup(void* data) {
    if (data) {
        free(data);
    }
}

// Keine spezifischen Events für Schüsse
static void handle_events(SDL_Event* event, void* data) {
    // Nichts zu tun
}

// Funktion zur Erstellung eines neuen Schusses
Entity create_shot_entity(SDL_Renderer* renderer, float x, float y) {
    if (!renderer) {
        printf("Fehler: Ungültiger Renderer!\n");
        Entity empty = {0};
        return empty;
    }

    // Textur laden, falls noch nicht geschehen
    if (!shot_texture) {
        const char path[] = "C:\\Users\\tb\\CLionProjects\\Spaceinvaders\\shot.png";
        shot_texture = IMG_LoadTexture(renderer, path);
        if (!shot_texture) {
            printf("Fehler: Konnte Schuss-Textur nicht laden: %s\n", SDL_GetError());
            Entity empty = {0};
            return empty;
        }
    }

    // Speicher für Schuss-Daten reservieren
    ShotData* shot = (ShotData*)malloc(sizeof(ShotData));
    if (!shot) {
        printf("Fehler: Speicherreservierung fehlgeschlagen!\n");
        Entity empty = {0};
        return empty;
    }

    // Schuss initialisieren
    shot->x = x;
    shot->y = y;
    shot->speed = 300.0f;
    shot->created = SDL_GetTicks();
    shot->id = next_shot_id++;
    shot->active = true;

    printf("Neuer Schuss #%d erstellt: x=%.2f, y=%.2f, speed=%.2f\n",
           shot->id, shot->x, shot->y, shot->speed);

    // Entity erstellen
    Entity entity = {
        .update = update,
        .render = render,
        .handle_events = handle_events,
        .cleanup = cleanup,
        .data = shot
    };

    return entity;
}