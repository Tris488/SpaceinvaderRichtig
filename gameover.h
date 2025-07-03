// gameover.h
#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "entity.h"

#define MAX_NAME_LENGTH 20
#define MAX_HIGHSCORES 5

// Highscore-Eintrag
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int score;
} HighscoreEntry;

// Game Over System initialisieren
Entity init_gameover_system(SDL_Renderer* renderer);

// Game Over Status
void set_game_over(bool game_over);
bool is_game_over(void);

// Spiel neu starten
void restart_game(void);

// Highscore-Funktionen
void load_highscores(void);
void save_highscores(void);
bool is_highscore(int score);
void add_highscore(const char* name, int score);
HighscoreEntry* get_highscores(void);

#endif // GAMEOVER_H