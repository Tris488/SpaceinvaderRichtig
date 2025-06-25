#ifndef ENEMY_H
#define ENEMY_H

#include <SDL3/SDL.h>
#include <stdbool.h>

// Maximale Anzahl aktiver Gegner
#define MAX_ENEMIES 100

// Gegnerkategorien
typedef enum {
    ENEMY_CATEGORY_SMALL,   // 1 Leben
    ENEMY_CATEGORY_MEDIUM,  // 2 Leben
    ENEMY_CATEGORY_LARGE    // 3 Leben
} EnemyCategory;

// Gegnertypen
typedef enum {
    // Kleine Gegner (1 Leben)
    ENEMY_RED,
    ENEMY_YELLOW,
    ENEMY_PINK,
    ENEMY_BLUE,

    // Mittlere Gegner (2 Leben)
    ENEMY_GOLD,
    ENEMY_DARKGREEN,
    ENEMY_SILVER,

    // Große Gegner (3 Leben)
    ENEMY_GREEN,

    ENEMY_TYPE_COUNT
} EnemyType;

// Gegner-Struktur
typedef struct {
    EnemyType type;
    EnemyCategory category;
    SDL_FRect position;      // Aktuelle Position auf dem Bildschirm
    SDL_FRect sprite_rect;   // Position im Spritesheet
    int health;
    int max_health;
    int points;              // Punkte beim Besiegen
    float speed;
    bool active;

    // Bewegung
    float velocity_x;
    float velocity_y;

    // Animation
    float animation_timer;
    int animation_frame;
} Enemy;

// Wellen-Struktur
typedef struct {
    int wave_number;
    int total_enemies;
    int enemies_spawned;
    int enemies_remaining;
    float spawn_timer;
    float spawn_delay;
    bool wave_complete;
} Wave;

// Initialisierung
void enemy_init_system(SDL_Renderer* renderer);
void enemy_cleanup_system(void);

// Gegner-Management
Enemy* enemy_spawn(EnemyType type, float x, float y);
void enemy_damage(Enemy* enemy, int damage);
void enemy_destroy(Enemy* enemy);
void enemy_update_all(float delta_time);
void enemy_render_all(SDL_Renderer* renderer);

// Kollision
bool check_collision(Enemy* enemy, SDL_FRect* bullet_rect);
Enemy* collision_with_bullet(SDL_FRect* bullet_rect);
Enemy* collision_with_player(SDL_FRect* player_rect);

// Spieler-Funktionen (extern definiert)
extern void damage_player(int damage);

// Wellen-System
void wave_init(int wave_number);
void wave_update(float delta_time);
bool wave_is_complete(void);
int wave_get_current(void);
void wave_start_next(void);

// Spieler-Score
void score_add(int points);
int score_get_current(void);
void score_reset(void);

// Utility
int enemy_count_active(void);
Enemy* enemy_get_all(void);
const SDL_FRect* enemy_get_sprite_rect(EnemyType type);

#endif // ENEMY_H