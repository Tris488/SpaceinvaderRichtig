// pew.c - Korrigierte Version
#include "pew.h"

// Texture für Schüsse
static SDL_Texture* shot_texture = NULL;
static SDL_FRect spriteshot = {0, 4, 15, 19};

// Zähler für Schuss-IDs
static int next_shot_id = 1;

// Update-Funktion für einen einzelnen Schuss
static void update(float delta_time, void* data) {
    if (!data) return;

    ShotData* shot = (ShotData*)data;

    // Ignoriere inaktive Schüsse
    if (!shot->active) return;

    // Alte Position speichern für Debug-Ausgabe
    float old_y = shot->y;

    // Schuss-Bewegung
    shot->y -= shot->speed * delta_time;

    // Debug-Ausgabe nur für diesen einen Schuss
    if (shot->id % 10 == 1) { // Nur jeden 10. Schuss loggen, um Spam zu vermeiden
        printf("Schuss #%d: y=%.2f → %.2f (delta_time: %.4f)\n",
                shot->id, old_y, shot->y, delta_time);
    }

    // Prüfe, ob der Schuss den Bildschirm verlassen hat
    if (shot->y < -20) {
        printf("Schuss #%d verlässt Bildschirm\n", shot->id);
        shot->active = false;

        // Markiere diese Entity zum Entfernen
        shot->should_remove = true;
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
        ShotData* shot = (ShotData*)data;
        printf("Schuss #%d wird aufgeräumt\n", shot->id);
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
    shot->speed = 300.0f; // Konstante Geschwindigkeit
    shot->created = SDL_GetTicks();
    shot->lasttick = shot->created;
    shot->time = 0.0f;
    shot->id = next_shot_id++;
    shot->active = true;
    shot->should_remove = false;

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

// Globale Funktion zum Aufräumen inaktiver Schüsse
void cleanup_inactive_shots() {
    int removed = 0;

    for (int i = 0; i < entities_count; i++) {
        if (!entities[i].data) continue;

        // Prüfen ob es ein Schuss ist (durch Vergleich der update-Funktion)
        if (entities[i].update == update) {
            ShotData* shot = (ShotData*)entities[i].data;

            if (shot->should_remove) {
                // Aufräumen
                if (entities[i].cleanup) {
                    entities[i].cleanup(entities[i].data);
                }

                // Entity aus dem Array entfernen
                for (int j = i; j < entities_count - 1; j++) {
                    entities[j] = entities[j + 1];
                }

                entities_count--;
                removed++;
                i--; // Index anpassen, da wir ein Element entfernt haben
            }
        }
    }

    if (removed > 0) {
        printf("%d inaktive Schüsse entfernt. Entities gesamt: %d\n", removed, entities_count);
    }
}