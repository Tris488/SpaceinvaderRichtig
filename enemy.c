#include "enemy.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL3_image/SDL_image.h>
#include "entity.h"
#include "mainfolder/wichitg.h"
#include "powerup.h"
// Für fmaxf
#ifndef fmaxf
#define fmaxf(a,b) ((a) > (b) ? (a) : (b))
#endif

// Globale Variablen
static Enemy enemies[MAX_ENEMIES];
static Wave current_wave;
static int player_score = 0;
static SDL_Texture* enemy_texture = NULL;

// Gegner-Definitionen
typedef struct {
    EnemyType type;
    EnemyCategory category;
    SDL_FRect sprite_rect;
    int max_health;
    int points;
    float speed;
} EnemyDefinition;

static const EnemyDefinition enemy_definitions[ENEMY_TYPE_COUNT] = {
    // Kleine Gegner (1 Leben)
    [ENEMY_RED] = {
        .type = ENEMY_RED,
        .category = ENEMY_CATEGORY_SMALL,
        .sprite_rect = {41, 0, 6, 7},  // x=41, y=0, w=47-41=6, h=7
        .max_health = 1,
        .points = 10,
        .speed = 25.0f  // Vorher 50.0f
    },
    [ENEMY_YELLOW] = {
        .type = ENEMY_YELLOW,
        .category = ENEMY_CATEGORY_SMALL,
        .sprite_rect = {56, 0, 7, 7},  // x=56, y=0, w=63-56=7, h=7
        .max_health = 1,
        .points = 15,
        .speed = 30.0f  // Vorher 60.0f
    },
    [ENEMY_PINK] = {
        .type = ENEMY_PINK,
        .category = ENEMY_CATEGORY_SMALL,
        .sprite_rect = {65, 0, 5, 7},  // x=65, y=0, w=70-65=5, h=7
        .max_health = 1,
        .points = 20,
        .speed = 35.0f  // Vorher 70.0f
    },
    [ENEMY_BLUE] = {
        .type = ENEMY_BLUE,
        .category = ENEMY_CATEGORY_SMALL,
        .sprite_rect = {80, 0, 7, 7},  // x=80, y=0, w=87-80=7, h=7
        .max_health = 1,
        .points = 25,
        .speed = 27.5f  // Vorher 55.0f
    },

    // Mittlere Gegner (2 Leben)
    [ENEMY_GOLD] = {
        .type = ENEMY_GOLD,
        .category = ENEMY_CATEGORY_MEDIUM,
        .sprite_rect = {41, 17, 13, 11},  // x=41, y=17, w=54-41=13, h=28-17=11
        .max_health = 2,
        .points = 50,
        .speed = 20.0f  // Vorher 40.0f
    },
    [ENEMY_DARKGREEN] = {
        .type = ENEMY_DARKGREEN,
        .category = ENEMY_CATEGORY_MEDIUM,
        .sprite_rect = {60, 17, 8, 13},  // x=60, y=17, w=68-60=8, h=30-17=13
        .max_health = 2,
        .points = 60,
        .speed = 22.5f  // Vorher 45.0f
    },
    [ENEMY_SILVER] = {
        .type = ENEMY_SILVER,
        .category = ENEMY_CATEGORY_MEDIUM,
        .sprite_rect = {72, 17, 15, 13},  // x=72, y=17, w=87-72=15, h=30-17=13
        .max_health = 2,
        .points = 70,
        .speed = 17.5f  // Vorher 35.0f
    },

    // Grosse Gegner (3 Leben)
    [ENEMY_GREEN] = {
        .type = ENEMY_GREEN,
        .category = ENEMY_CATEGORY_LARGE,
        .sprite_rect = {40, 41, 30, 22},  // x=40, y=41, w=70-40=30, h=63-41=22
        .max_health = 3,
        .points = 100,
        .speed = 12.5f  // Vorher 25.0f
    }
};

// Wellen-Definitionen
typedef struct {
    int wave_number;
    int enemy_count[ENEMY_TYPE_COUNT];
    float spawn_delay;
} WaveDefinition;

static const WaveDefinition wave_definitions[] = {
    // Welle 1: Nur kleine Gegner
    {
        .wave_number = 1,
        .enemy_count = {
            [ENEMY_RED] = 5,
            [ENEMY_YELLOW] = 3,
            [ENEMY_PINK] = 0,
            [ENEMY_BLUE] = 2,
            [ENEMY_GOLD] = 0,
            [ENEMY_DARKGREEN] = 0,
            [ENEMY_SILVER] = 0,
            [ENEMY_GREEN] = 0
        },
        .spawn_delay = 1.0f
    },
    // Welle 2: Kleine und mittlere Gegner
    {
        .wave_number = 2,
        .enemy_count = {
            [ENEMY_RED] = 3,
            [ENEMY_YELLOW] = 4,
            [ENEMY_PINK] = 2,
            [ENEMY_BLUE] = 3,
            [ENEMY_GOLD] = 2,
            [ENEMY_DARKGREEN] = 1,
            [ENEMY_SILVER] = 0,
            [ENEMY_GREEN] = 0
        },
        .spawn_delay = 0.8f
    },
    // Welle 3: Alle Gegnertypen
    {
        .wave_number = 3,
        .enemy_count = {
            [ENEMY_RED] = 2,
            [ENEMY_YELLOW] = 3,
            [ENEMY_PINK] = 3,
            [ENEMY_BLUE] = 2,
            [ENEMY_GOLD] = 2,
            [ENEMY_DARKGREEN] = 2,
            [ENEMY_SILVER] = 1,
            [ENEMY_GREEN] = 1
        },
        .spawn_delay = 0.6f
    }
};

static const int wave_count = sizeof(wave_definitions) / sizeof(wave_definitions[0]);

// Initialisierung
void enemy_init_system(SDL_Renderer* renderer) {
    memset(enemies, 0, sizeof(enemies));
    memset(&current_wave, 0, sizeof(current_wave));
    player_score = 0;

    // Lade die Enemy-Textur
    if (!enemy_texture) {
        const char path[] = "C:\\Users\\tb\\CLionProjects\\SpaceinvaderRichtig1\\pictures\\pico8_invaders_sprites_LARGE.png";
        enemy_texture = IMG_LoadTexture(renderer, path);
        if (!enemy_texture) {
            printf("Failed to load enemy texture: %s\n", SDL_GetError());
        }
    }
}

void enemy_cleanup_system(void) {
    // Alle Gegner deaktivieren
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
    }

    // Textur freigeben
    if (enemy_texture) {
        SDL_DestroyTexture(enemy_texture);
        enemy_texture = NULL;
    }
}

// Gegner spawnen
Enemy* enemy_spawn(EnemyType type, float x, float y) {
    // Freien Slot finden
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            Enemy* enemy = &enemies[i];
            const EnemyDefinition* def = &enemy_definitions[type];

            // Basis-Eigenschaften setzen
            enemy->type = type;
            enemy->category = def->category;
            enemy->sprite_rect = def->sprite_rect;
            enemy->max_health = def->max_health;
            enemy->health = def->max_health;
            enemy->points = def->points;
            enemy->speed = def->speed;
            enemy->active = true;

            // Position setzen
            enemy->position.x = x;
            enemy->position.y = y;
            enemy->position.w = def->sprite_rect.w * 4; // Groessere Skalierung (vorher 2)
            enemy->position.h = def->sprite_rect.h * 4;

            // Bewegung initialisieren (Beispiel: nach unten)
            enemy->velocity_x = 0;
            enemy->velocity_y = enemy->speed;

            // Animation
            enemy->animation_timer = 0;
            enemy->animation_frame = 0;

            return enemy;
        }
    }
    return NULL;
}

// Gegner Schaden zufuegen
void enemy_damage(Enemy* enemy, int damage) {
    if (!enemy || !enemy->active) return;

    enemy->health -= damage;

    if (enemy->health <= 0) {
        enemy_destroy(enemy);
    }
}

// Gegner zerstoeren
void enemy_destroy(Enemy* enemy) {
    if (!enemy || !enemy->active) return;

    // Punkte zum Score hinzufuegen
    score_add(enemy->points);
    powerup_on_enemy_killed(enemy->position.x + enemy->position.w/2,
                           enemy->position.y + enemy->position.h/2);
    // Gegner deaktivieren
    enemy->active = false;

    // Wellen-Zaehler aktualisieren
    if (current_wave.enemies_remaining > 0) {
        current_wave.enemies_remaining--;
    }
}
void destroy_all_enemies(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Zerstöre jeden aktiven Gegner
            enemy_destroy(&enemies[i]);
        }
    }
}
// Alle Gegner aktualisieren
void enemy_update_all(float delta_time) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            Enemy* enemy = &enemies[i];

            // Position aktualisieren
            enemy->position.x += enemy->velocity_x * delta_time;
            enemy->position.y += enemy->velocity_y * delta_time;

            // Pruefe ob Gegner unten durchgekommen ist
            if (enemy->position.y > 600) {
                printf("Gegner ist durchgekommen! Spieler verliert ein Leben.\n");
                enemy->active = false;
                current_wave.enemies_remaining--;

                // Spieler verliert ein Leben
                damage_player(1);
            }

            // Animation aktualisieren
            enemy->animation_timer += delta_time;
        }
    }
}

// Alle Gegner rendern
void enemy_render_all(SDL_Renderer* renderer) {
    if (!enemy_texture) return;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            Enemy* enemy = &enemies[i];

            SDL_FRect src = {
                enemy->sprite_rect.x,
                enemy->sprite_rect.y,
                enemy->sprite_rect.w,
                enemy->sprite_rect.h
            };

            SDL_FRect dst = {
                enemy->position.x,
                enemy->position.y,
                enemy->position.w,
                enemy->position.h
            };

            SDL_SetTextureScaleMode(enemy_texture, SDL_SCALEMODE_NEAREST);
            SDL_RenderTexture(renderer, enemy_texture, &src, &dst);
        }
    }
}

// Kollisionspruefung
bool check_collision(Enemy* enemy, SDL_FRect* bullet_rect) {
    if (!enemy || !enemy->active || !bullet_rect) return false;

    return (enemy->position.x < bullet_rect->x + bullet_rect->w &&
            enemy->position.x + enemy->position.w > bullet_rect->x &&
            enemy->position.y < bullet_rect->y + bullet_rect->h &&
            enemy->position.y + enemy->position.h > bullet_rect->y);
}

Enemy* collision_with_bullet(SDL_FRect* bullet_rect) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active && check_collision(&enemies[i], bullet_rect)) {
            return &enemies[i];
        }
    }
    return NULL;
}

// Neue Funktion: Kollision mit Spieler pruefen
Enemy* collision_with_player(SDL_FRect* player_rect) {
    if (!player_rect) return NULL;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active && check_collision(&enemies[i], player_rect)) {
            return &enemies[i];
        }
    }
    return NULL;
}

// Wellen-System mit unendlichen Wellen
void wave_init(int wave_number) {
    if (wave_number <= 0) {
        wave_number = 1;
    }

    current_wave.wave_number = wave_number;
    current_wave.total_enemies = 0;
    current_wave.enemies_spawned = 0;
    current_wave.spawn_timer = 0;
    current_wave.wave_complete = false;

    // Dynamische Schwierigkeit basierend auf Wellennummer
    if (wave_number <= 3) {
        // Erste 3 Wellen wie definiert
        const WaveDefinition* wave_def = &wave_definitions[wave_number - 1];
        current_wave.spawn_delay = wave_def->spawn_delay;

        // Kopiere Gegnerzahlen
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            int count = wave_def->enemy_count[i];
            current_wave.total_enemies += count;
        }
    } else {
        // Unendliche Wellen mit steigender Schwierigkeit
        int difficulty = wave_number - 3; // Schwierigkeit steigt nach Welle 3

        // Spawn-Delay wird schneller (minimum 0.2 Sekunden)
        current_wave.spawn_delay = fmaxf(0.2f, 0.6f - (difficulty * 0.05f));

        // Basis-Gegnerzahlen fuer unendliche Wellen
        int base_small = 3 + difficulty;      // Kleine Gegner
        int base_medium = 1 + (difficulty / 2); // Mittlere Gegner
        int base_large = (difficulty / 3);      // Grosse Gegner

        // Zufaellige Variation
        int small_enemies = base_small + (rand() % 3);
        int medium_enemies = base_medium + (rand() % 2);
        int large_enemies = base_large + (rand() % 2);

        // Verteile kleine Gegner auf die 4 Typen
        int small_per_type = small_enemies / 4;
        int small_remainder = small_enemies % 4;

        // Setze Gegnerzahlen
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            if (i <= ENEMY_BLUE) { // Kleine Gegner
                int count = small_per_type;
                if (i < small_remainder) count++; // Verteile Rest
                current_wave.total_enemies += count;
            } else if (i <= ENEMY_SILVER) { // Mittlere Gegner
                int count = medium_enemies / 3; // 3 mittlere Typen
                if (i == ENEMY_GOLD && medium_enemies % 3 > 0) count++;
                current_wave.total_enemies += count;
            } else { // Grosse Gegner
                current_wave.total_enemies += large_enemies;
            }
        }

        printf("Wave %d: %d enemies (S:%d M:%d L:%d), delay:%.2f\n",
               wave_number, current_wave.total_enemies,
               small_enemies, medium_enemies, large_enemies,
               current_wave.spawn_delay);
    }

    current_wave.enemies_remaining = current_wave.total_enemies;
    printf("Wave %d initialized with %d enemies\n", wave_number, current_wave.total_enemies);
}

void wave_update(float delta_time) {
    if (current_wave.wave_complete) return;

    // Spawn-Timer aktualisieren
    current_wave.spawn_timer += delta_time;

    if (current_wave.spawn_timer >= current_wave.spawn_delay &&
        current_wave.enemies_spawned < current_wave.total_enemies) {

        current_wave.spawn_timer = 0;

        // Arrays fuer verbleibende Gegner
        static int spawned_per_type[ENEMY_TYPE_COUNT] = {0};
        static int needed_per_type[ENEMY_TYPE_COUNT] = {0};

        // Initialisiere bei neuer Welle
        if (current_wave.enemies_spawned == 0) {
            for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
                spawned_per_type[i] = 0;
                needed_per_type[i] = 0;
            }

            // Berechne wie viele von jedem Typ gebraucht werden
            if (current_wave.wave_number <= 3) {
                // Erste 3 Wellen verwenden vordefinierte Werte
                const WaveDefinition* wave_def = &wave_definitions[current_wave.wave_number - 1];
                for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
                    needed_per_type[i] = wave_def->enemy_count[i];
                }
            } else {
                // Unendliche Wellen - dynamische Verteilung
                int difficulty = current_wave.wave_number - 3;
                int remaining = current_wave.total_enemies;

                // Verteile Gegner auf Typen
                for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
                    if (i <= ENEMY_BLUE) { // Kleine Gegner (40-60% der Welle)
                        needed_per_type[i] = remaining / (8 - i);
                    } else if (i <= ENEMY_SILVER) { // Mittlere Gegner (30-40%)
                        needed_per_type[i] = remaining / (12 - i);
                    } else { // Grosse Gegner (10-20%)
                        needed_per_type[i] = remaining / 10;
                    }
                    remaining -= needed_per_type[i];
                }
                // Rest auf kleine Gegner verteilen
                if (remaining > 0) {
                    needed_per_type[ENEMY_RED] += remaining;
                }
            }
        }

        // Liste der verfuegbaren Gegnertypen erstellen
        int available_types[ENEMY_TYPE_COUNT];
        int available_count = 0;

        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            if (spawned_per_type[i] < needed_per_type[i]) {
                available_types[available_count++] = i;
            }
        }

        // Zufaellig einen verfuegbaren Typ auswaehlen
        if (available_count > 0) {
            int random_index = rand() % available_count;
            int enemy_type = available_types[random_index];

            // Zufaellige X-Position
            float x = (float)(rand() % 700) + 50;
            float y = -50;

            // Bei hoeheren Wellen: Gegner koennen schneller sein
            Enemy* spawned = enemy_spawn(enemy_type, x, y);
            if (spawned && current_wave.wave_number > 5) {
                // Geschwindigkeit erhoehen basierend auf Welle
                float speed_multiplier = 1.0f + (current_wave.wave_number - 5) * 0.1f;
                spawned->speed *= speed_multiplier;
                spawned->velocity_y = spawned->speed;
            }

            if (spawned) {
                spawned_per_type[enemy_type]++;
                current_wave.enemies_spawned++;
            }
        }
    }

    // Pruefen ob Welle abgeschlossen
    if (current_wave.enemies_remaining == 0 && current_wave.enemies_spawned >= current_wave.total_enemies) {
        current_wave.wave_complete = true;
        printf("Wave %d complete! Score: %d\n", current_wave.wave_number, score_get_current());
    }
}

bool wave_is_complete(void) {
    return current_wave.wave_complete;
}

int wave_get_current(void) {
    return current_wave.wave_number;
}

void wave_start_next(void) {
    int next_wave = current_wave.wave_number + 1;
    // Kein Limit mehr - unendliche Wellen!
    wave_init(next_wave);
}

// Score-System
void score_add(int points) {
    player_score += points;
}



int score_get_current(void) {
    return player_score;
}

void score_reset(void) {
    player_score = 0;
}

// Utility-Funktionen
int enemy_count_active(void) {
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) count++;
    }
    return count;
}

Enemy* enemy_get_all(void) {
    return enemies;
}

const SDL_FRect* enemy_get_sprite_rect(EnemyType type) {
    if (type >= 0 && type < ENEMY_TYPE_COUNT) {
        return &enemy_definitions[type].sprite_rect;
    }
    return NULL;
}