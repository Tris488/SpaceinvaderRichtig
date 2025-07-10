//
// Created by User on 08.07.2025.
//

#ifndef DATABASE_H
#define DATABASE_H

#include "gameover.h"

int initDB();
int db_load_highscores(HighscoreEntry *highscores);
int db_is_highscore(int score);
int db_save_highscore(char *name, int score);
void closeDB();

#endif //DATABASE_H
