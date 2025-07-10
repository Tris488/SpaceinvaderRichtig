//
// Created by User on 08.07.2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "database.h"
#include "gameover.h"

#ifndef MYSQL_SSL_MODE_DISABLED
#define MYSQL_SSL_MODE_DISABLED 2
#endif

MYSQL *conn = NULL;

// Hilfsfunktion um Verbindung zu prüfen
int checkConnection() {
    if (conn == NULL) {
        fprintf(stderr, "Keine Datenbankverbindung vorhanden\n");
        return 0;
    }

    // Prüfe ob Verbindung noch aktiv ist
    if (mysql_ping(conn) != 0) {
        fprintf(stderr, "Datenbankverbindung verloren: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

//check/init database connection
int initDB() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 0;
    }

    int ssl_mode = MYSQL_SSL_MODE_DISABLED;
    mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);

    // Debugging-Ausgabe
    printf("DEBUG: Versuche Verbindung zu 213.165.95.218:3306 als 'kloss'\n");

    if (mysql_real_connect(conn, "213.165.95.218", "kloss", "sql", "arcade", 3306, NULL, 0) == NULL) {
        fprintf(stderr, "Verbindung fehlgeschlagen: %s\n", mysql_error(conn));
        mysql_close(conn);
        conn = NULL;
        return 0;
    }

    printf("DEBUG: Datenbankverbindung erfolgreich hergestellt\n");
    return 1;
}

int db_load_highscores(HighscoreEntry *highscores) {
    if (!checkConnection()) {
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT name, score FROM highscore ORDER BY score DESC LIMIT %d", MAX_HIGHSCORES);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage (load_highscores): %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Fehler beim Abrufen der Ergebnisse: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_ROW row;
    int i = 0;
    while ((row = mysql_fetch_row(result)) && i < MAX_HIGHSCORES) {
        strncpy(highscores[i].name, row[0], MAX_NAME_LENGTH);
        highscores[i].name[MAX_NAME_LENGTH] = '\0';
        highscores[i].score = atoi(row[1]);
        i++;
    }
    mysql_free_result(result);
    return 1;
}

int db_is_highscore(int score) {
    if (!checkConnection()) {
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT score FROM highscore ORDER BY score DESC LIMIT %d", MAX_HIGHSCORES);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage (db_is_highscore): %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Fehler beim Abrufen der Ergebnisse: %s\n", mysql_error(conn));
        return 0;
    }

    int num_rows = mysql_num_rows(result);

    // Weniger als MAX_HIGHSCORES Einträge → auf jeden Fall ein Highscore
    if (num_rows < MAX_HIGHSCORES) {
        mysql_free_result(result);
        return 1;
    }

    MYSQL_ROW row;
    int i = 0;
    while ((row = mysql_fetch_row(result)) && i < MAX_HIGHSCORES) {
        int existing_score = atoi(row[0]);
        if (score > existing_score) {
            mysql_free_result(result);
            return 1;
        }
        i++;
    }

    mysql_free_result(result);
    return 0; // Kein Highscore
}

int db_save_highscore(char *name, int score) {
    if (!checkConnection()) {
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "INSERT INTO highscore (name, score) VALUES ('%s', %d)",
        name, score);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler beim Speichern des Highscores: %s\n", mysql_error(conn));
        return 0;
    }

    printf("DEBUG: Highscore gespeichert: %s - %d\n", name, score);
    return 1;
}

void closeDB() {
    if (conn != NULL) {
        mysql_close(conn);
        conn = NULL;
    }
}