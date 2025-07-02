// gameover.c
#include "gameover.h"
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enemy.h"
#include "health.h"
#include "powerup.h"

// Globale Variablen
static bool game_over = false;
static SDL_Texture* gameover_texture = NULL;
static SDL_Texture* font_texture = NULL;
static bool restart_requested = false;
static HighscoreEntry highscores[MAX_HIGHSCORES];
static bool highscores_loaded = false;

// Eingabe für neuen Highscore
static bool entering_name = false;
static char player_name[MAX_NAME_LENGTH + 1] = "";
static int name_cursor = 0;
static int player_final_score = 0;
static bool score_checked = false;  // Neu: verhindert mehrfache Prüfung
static bool processing_input = false; // Neu: verhindert mehrfache Event-Verarbeitung

// Font-Rendering (einfache ASCII-Schriftart)
typedef struct {
    int x, y, w, h;
} CharRect;

// Einfache ASCII-Zeichen-Positionen (für Basis-Font)
static CharRect get_char_rect(char c) {
    CharRect rect = {0, 0, 8, 8};

    // Zahlen
    if (c >= '0' && c <= '9') {
        rect.x = (c - '0') * 8;
        rect.y = 0;
    }
    // Großbuchstaben
    else if (c >= 'A' && c <= 'Z') {
        rect.x = ((c - 'A') % 16) * 8;
        rect.y = ((c - 'A') / 16 + 1) * 8;
    }
    // Kleinbuchstaben
    else if (c >= 'a' && c <= 'z') {
        rect.x = ((c - 'a') % 16) * 8;
        rect.y = ((c - 'a') / 16 + 3) * 8;
    }
    // Leerzeichen und andere
    else {
        rect.x = 0;
        rect.y = 0;
    }

    return rect;
}

// Text rendern
static void render_text(SDL_Renderer* renderer, const char* text, int x, int y, float scale) {
    if (!text) return;

    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        char c = text[i];

        // Verwende SDL_RenderDebugText für einfache Textausgabe
        char single_char[2] = {c, '\0'};
        SDL_RenderDebugText(renderer, x + i * 20 * scale, y, single_char);
    }
}

// Highscores laden
void load_highscores(void) {
    FILE* file = fopen("highscore.txt", "r");

    // Initialisiere mit Default-Werten
    for (int i = 0; i < MAX_HIGHSCORES; i++) {
        strcpy(highscores[i].name, "---");
        highscores[i].score = 0;
    }

    if (file) {
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            char line[256];
            if (fgets(line, sizeof(line), file)) {
                // Parse "NAME SCORE"
                char* space = strrchr(line, ' ');
                if (space) {
                    *space = '\0';
                    strncpy(highscores[i].name, line, MAX_NAME_LENGTH);
                    highscores[i].name[MAX_NAME_LENGTH] = '\0';
                    highscores[i].score = atoi(space + 1);
                }
            }
        }
        fclose(file);
        printf("Highscores geladen.\n");
    } else {
        printf("Keine highscore.txt gefunden, verwende Default-Werte.\n");
    }

    highscores_loaded = true;
}

// Highscores speichern
void save_highscores(void) {
    FILE* file = fopen("highscore.txt", "w");
    if (file) {
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            fprintf(file, "%s %d\n", highscores[i].name, highscores[i].score);
        }
        fclose(file);
        printf("Highscores gespeichert.\n");
    } else {
        printf("FEHLER: Konnte highscore.txt nicht speichern!\n");
    }
}

// Prüfen ob Score ein Highscore ist
bool is_highscore(int score) {
    if (!highscores_loaded) {
        load_highscores();
    }

    // Prüfe ob Score in Top 5 kommt
    return score > highscores[MAX_HIGHSCORES - 1].score;
}

// Highscore hinzufügen
void add_highscore(const char* name, int score) {
    if (!highscores_loaded) {
        load_highscores();
    }

    // Finde Position für neuen Score
    int insert_pos = MAX_HIGHSCORES;
    for (int i = 0; i < MAX_HIGHSCORES; i++) {
        if (score > highscores[i].score) {
            insert_pos = i;
            break;
        }
    }

    // Verschiebe niedrigere Scores nach unten
    if (insert_pos < MAX_HIGHSCORES) {
        for (int i = MAX_HIGHSCORES - 1; i > insert_pos; i--) {
            highscores[i] = highscores[i - 1];
        }

        // Füge neuen Score ein
        strncpy(highscores[insert_pos].name, name, MAX_NAME_LENGTH);
        highscores[insert_pos].name[MAX_NAME_LENGTH] = '\0';
        highscores[insert_pos].score = score;

        save_highscores();
    }
}

// Highscores zurückgeben
HighscoreEntry* get_highscores(void) {
    if (!highscores_loaded) {
        load_highscores();
    }
    return highscores;
}

// Update-Funktion
static void update(float delta_time, void* data) {
    if (!game_over) return;

    // Prüfe ob Spieler einen neuen Highscore hat (nur einmal!)
    if (!entering_name && !score_checked) {
        if (score_get_current() > 0 && is_highscore(score_get_current())) {
            entering_name = true;
            player_final_score = score_get_current();
            memset(player_name, 0, sizeof(player_name));
            name_cursor = 0;
            printf("Neuer Highscore! Bitte Namen eingeben.\n");
        }
        score_checked = true;  // Setze Flag IMMER, auch wenn kein Highscore
    }

    // Neustart-Anfrage verarbeiten
    if (restart_requested) {
        restart_game();
        restart_requested = false;
    }
}

// Render-Funktion
static void render(SDL_Renderer* renderer, void* data) {
    if (!game_over) return;

    // Dunkler Hintergrund-Overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_FRect overlay = {0, 0, 800, 600};
    SDL_RenderFillRect(renderer, &overlay);

    if (entering_name) {
        // Namenseingabe-Bildschirm
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Titel
        SDL_RenderDebugText(renderer, 250, 100, "NEUER HIGHSCORE!");

        // Score anzeigen
        char score_text[64];
        snprintf(score_text, sizeof(score_text), "Score: %d", player_final_score);
        SDL_RenderDebugText(renderer, 300, 150, score_text);

        // Namenseingabe
        SDL_RenderDebugText(renderer, 200, 250, "Name eingeben:");

        // Eingabefeld
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_FRect input_box = {250, 300, 300, 40};
        SDL_RenderRect(renderer, &input_box);

        // Eingegebenen Namen anzeigen
        if (strlen(player_name) > 0) {
            SDL_RenderDebugText(renderer, 260, 310, player_name);
        }

        // Cursor
        if ((SDL_GetTicks() / 500) % 2 == 0) {
            int cursor_x = 260 + name_cursor * 15;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderLine(renderer, cursor_x, 310, cursor_x, 330);
        }

        // Anweisungen
        SDL_RenderDebugText(renderer, 200, 400, "ENTER - Bestaetigen");
        SDL_RenderDebugText(renderer, 200, 430, "BACKSPACE - Loeschen");
    } else {
        // Game Over Bildschirm mit Highscores
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Game Over Text
        SDL_RenderDebugText(renderer, 300, 50, "GAME OVER");

        // Aktueller Score
        char score_text[64];
        snprintf(score_text, sizeof(score_text), "Dein Score: %d", score_get_current());
        SDL_RenderDebugText(renderer, 250, 100, score_text);

        // Highscore-Tabelle
        SDL_RenderDebugText(renderer, 300, 180, "TOP 5 HIGHSCORES");

        // Trennlinie
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderLine(renderer, 200, 210, 600, 210);

        // Highscores anzeigen
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            char entry[128];
            snprintf(entry, sizeof(entry), "%d. %-20s %6d",
                    i + 1, highscores[i].name, highscores[i].score);

            int y = 230 + i * 40;

            // Highlight wenn es der aktuelle Score ist
            if (highscores[i].score == score_get_current() &&
                strcmp(highscores[i].name, player_name) == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }

            SDL_RenderDebugText(renderer, 200, y, entry);
        }

        // Neustart-Option
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer, 250, 500, "ENTER - Neues Spiel");
        SDL_RenderDebugText(renderer, 250, 530, "ESC - Beenden");
    }
}

// Event-Handler
static void handle_events(SDL_Event* event, void* data) {
    if (!game_over) return;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        // Ignoriere Key-Repeat Events
        if (event->key.repeat) {
            return;
        }

        if (entering_name||!score_checked) {
            // Namenseingabe behandeln
            SDL_Keycode key = event->key.key;

            if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                // Namen bestätigen - nur einmal verarbeiten
                if (strlen(player_name) > 0 && !processing_input) {
                    processing_input = true;
                    add_highscore(player_name, player_final_score);
                    entering_name = false;
                    processing_input = false;
                    printf("Highscore gespeichert: %s - %d\n", player_name, player_final_score);
                }
            }
            else if (key == SDLK_BACKSPACE) {
                // Zeichen löschen
                if (name_cursor > 0) {
                    name_cursor--;
                    player_name[name_cursor] = '\0';
                }
            }
            else if (name_cursor < MAX_NAME_LENGTH) {
                // Buchstaben und Zahlen
                char c = '\0';

                if (key >= SDLK_A && key <= SDLK_Z) {
                    c = 'A' + (key - SDLK_A);
                }
                else if (key >= SDLK_0 && key <= SDLK_9) {
                    c = '0' + (key - SDLK_0);
                }
                else if (key == SDLK_SPACE) {
                    c = ' ';
                }

                if (c != '\0') {
                    player_name[name_cursor] = c;
                    name_cursor++;
                    player_name[name_cursor] = '\0';
                }
            }
        }
        else {
            // Game Over Bildschirm
            if (event->key.key == SDLK_RETURN || event->key.key == SDLK_KP_ENTER) {
                restart_requested = true;
            }
            else if (event->key.key == SDLK_ESCAPE) {
                // Beende das Spiel
                SDL_Event quit_event;
                quit_event.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&quit_event);
            }
        }
    }
}

// Cleanup
static void cleanup(void* data) {
    if (gameover_texture) {
        SDL_DestroyTexture(gameover_texture);
        gameover_texture = NULL;
    }
    if (font_texture) {
        SDL_DestroyTexture(font_texture);
        font_texture = NULL;
    }
}

// Game Over Status setzen
void set_game_over(bool new_game_over) {
    game_over = new_game_over;
    if (game_over) {
        printf("=== GAME OVER ===\n");
        printf("Final Score: %d\n", score_get_current());

        // Lade Highscores falls noch nicht geladen
        if (!highscores_loaded) {
            load_highscores();
        }

        // Reset score_checked für neue Game Over
        score_checked = false;
    }
}

// Game Over Status abfragen
bool is_game_over(void) {
    return game_over;
}

// Spiel neu starten
void restart_game(void) {
    printf("Starte Spiel neu...\n");

    // Reset game state
    game_over = false;
    entering_name = false;
    score_checked = false;  // Reset flag
    memset(player_name, 0, sizeof(player_name));
    name_cursor = 0;

    // Reset player
    set_player_health(3);

    // Reset enemies und score
    score_reset();

    // Reset powerups
    Enemy* all_enemies = enemy_get_all();
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (all_enemies[i].active) {
            all_enemies[i].active = false;
        }
    }

    // Welle 1 neu starten
    wave_init(1);
    powerup_reset_all();


    // Wichtig: Enemy System muss neu initialisiert werden
    // Das passiert automatisch in enemy_entity.c wenn wave_started auf false gesetzt wird

    printf("Neustart abgeschlossen.\n");
}

// Entity initialisieren
Entity init_gameover_system(SDL_Renderer* renderer) {
    printf("Initialisiere Game Over System...\n");

    // Lade Highscores beim Start
    load_highscores();

    Entity gameover_entity = {
        .update = update,
        .render = render,
        .handle_events = handle_events,
        .cleanup = cleanup,
        .data = NULL
    };

    return gameover_entity;
}