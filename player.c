//Player.c
#include "player.h"
#include <stdlib.h>
#include <stdio.h>
#include "enemy.h"
#include "health.h"
#include "gameover.h"
#include "powerup.h"
#include "joystick_manager.h"  // NEU: Joystick-Manager einbinden

static SDL_Texture* player_texture;
static SDL_FRect spriteplayer_portion = {0, 1, 8, 8};
// ENTFERNT: Alte Joystick-Variablen
int window_width, window_height;
static Uint32 last_shot_time = 0;
static Uint32 shot_cooldown = 900;
Position position = {0,0}; // Start at the bottom center of screen
static float move_speed = 200.0f; // Movement speed
static bool e_key_was_pressed = false;
static bool b_button_was_pressed = false;

// ENTFERNT: connect_joystick() Funktion - wird jetzt vom Manager übernommen

static void cleanup(void* data) {
    // ENTFERNT: Joystick cleanup - wird jetzt vom Manager übernommen
    if (player_texture) {
        SDL_DestroyTexture(player_texture);
        player_texture = NULL;
    }
}

// Process events
static void handle_events(SDL_Event* event, void* data) {
    if (!event) return;

    // NEU: Joystick-Events an Manager weiterleiten
    joystick_handle_event(event);

    // ENTFERNT: Alte Joystick-Event-Handler
}

static void update(float delta_time, void* data) {
    Uint32 effective_cooldown;
    float effective_movespeed;
    if (is_game_over()) return;

    // NEU: Joystick-Manager updaten (wichtig für just_pressed/released)
    joystick_update();

    // data ist der AppState pointer
    if (!data) return;

    AppState* state = (AppState*)data;
    if (!state->renderer) return;

    SDL_Renderer* renderer = state->renderer;

    // Keyboard controls
    Uint32 current_shot_time = SDL_GetTicks();
    const bool *keyboard_state = SDL_GetKeyboardState(NULL);

    if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP]) {
        effective_movespeed=move_speed*get_speed_multplier();
        position.y -= effective_movespeed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) {
        effective_movespeed=move_speed*get_speed_multplier();
        position.y += effective_movespeed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) {
        effective_movespeed=move_speed*get_speed_multplier();
        position.x -= effective_movespeed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) {
        effective_movespeed=move_speed*get_speed_multplier();
        position.x += effective_movespeed * delta_time;
    }

    // NEU: Joystick controls
    if (joystick_is_connected()) {
        // Bewegung mit dem Stick
        float axis_x = joystick_get_axis_normalized(0); // X-Achse (links/rechts)
        float axis_y = joystick_get_axis_normalized(1); // Y-Achse (vor/zurück)

        effective_movespeed = move_speed * get_speed_multplier();
        position.x += axis_x * effective_movespeed * delta_time;
        position.y += axis_y * effective_movespeed * delta_time;

        // Optional: Twist für zusätzliche seitliche Bewegung
        /*float twist = joystick_get_axis_normalized(2); // Z-Rotation
        position.x += twist * effective_movespeed * 0.5f * delta_time;
*/
        // Schießen mit Trigger (Button 0)
        if (joystick_is_button_pressed(0)) {
            effective_cooldown = (Uint32)(shot_cooldown * get_double_shoot_multiplier());
            if (current_shot_time > last_shot_time + effective_cooldown) {
                if (entities_count < MAX_ENTITIES) {
                    printf("Spieler feuert Schuss ab (Joystick): x=%.2f, y=%.2f\n", position.x, position.y);

                    // Schuss erstellen
                    Entity new_shot = create_shot_entity(renderer, position.x + 10, position.y - 20);

                    // Nur hinzufügen, wenn der Schuss erfolgreich erstellt wurde
                    if (new_shot.data != NULL) {
                        entities[entities_count] = new_shot;
                        entities_count++;
                        last_shot_time = current_shot_time;
                    }
                }
            }
        }

        // Bombe mit Daumen-Button (Button 1)
        bool b_button_pressed = joystick_is_button_pressed(1);
        if (b_button_pressed && !b_button_was_pressed) {
            if (get_bomb_count() > 0) {
                bomb_got_used();
                destroy_all_enemies();
                printf("Joystick: Bombe ausgelöst! Verbleibend: %d\n", get_bomb_count());
            }
        }
        b_button_was_pressed = b_button_pressed;
    }

    // Shooting (Keyboard)
    if (keyboard_state[SDL_SCANCODE_SPACE]) {
        effective_cooldown = (Uint32)(shot_cooldown * get_double_shoot_multiplier());
        if (current_shot_time > last_shot_time + effective_cooldown) {
            if (entities_count < MAX_ENTITIES) {
                printf("Spieler feuert Schuss ab: x=%.2f, y=%.2f\n", position.x, position.y);

                // Schuss erstellen
                Entity new_shot = create_shot_entity(renderer, position.x + 10, position.y - 20);

                // Nur hinzufügen, wenn der Schuss erfolgreich erstellt wurde
                if (new_shot.data != NULL) {
                    entities[entities_count] = new_shot;
                    entities_count++;
                    last_shot_time = current_shot_time;
                }
            }
        }
    }

    // Bomb functionality
    bool e_key_pressed = keyboard_state[SDL_SCANCODE_E];
    if (e_key_pressed && !e_key_was_pressed) {
        // E-Taste wurde gerade gedrückt
        if (get_bomb_count() > 0) {
            bomb_got_used();
            destroy_all_enemies();
            printf("Bombe ausgelöst! Verbleibend: %d\n", get_bomb_count());
        }
    }
    e_key_was_pressed = e_key_pressed;

    SDL_FRect player_rect = {position.x, position.y, 20, 40};
    Enemy* colliding_enemy = collision_with_player(&player_rect);
    Powerup* collected = powerup_check_collision(&player_rect);
    if (collected) {
        printf("Double Shoot Power-Up collected!\n");
    }
    if (colliding_enemy) {
        // Spieler wurde von Gegner getroffen
        damage_player(1);
        enemy_destroy(colliding_enemy); // Gegner wird zerstört

        // Optional: Kurze Unverwundbarkeit nach Treffer
        static Uint32 last_hit_time = 0;
        Uint32 current_time = SDL_GetTicks();

        if (current_time - last_hit_time > 1000) { // 1 Sekunde Unverwundbarkeit
            last_hit_time = current_time;
        }
    }

    // Keep player within screen bounds
    if (position.x < 0) {
        position.x = 0;
    }
    if (position.x > window_width - 20) {
        position.x = window_width - 20;
    }
    if (position.y < 60) {
        position.y = 60;
    }
    if (position.y > window_height - 40) {
        position.y = window_height - 40;
    }
}

static void render(SDL_Renderer* renderer, void* data) {
    if (!renderer || !player_texture) return;

    SDL_FRect player_position = {position.x, position.y, 20, 40};
    SDL_SetTextureScaleMode(player_texture, SDL_SCALEMODE_NEAREST);
    SDL_RenderTexture(renderer, player_texture, &spriteplayer_portion, &player_position);
}

Entity init_player(SDL_Renderer* renderer) {
    const char path[] =  "C:\\Users\\tb\\CLionProjects\\SpaceinvaderRichtig1\\pictures\\pico8_invaders_sprites_LARGE.png";
    player_texture = IMG_LoadTexture(renderer, path);

    if (!player_texture) {
        printf("Failed to load player texture: %s\n", SDL_GetError());
    }

    // NEU: Deadzone für Logitech Extreme 3D Pro erhöhen
    joystick_set_deadzone(0.35f);  // 20% Deadzone

    SDL_GetCurrentRenderOutputSize(renderer, &window_width, &window_height);
    position.x = window_width / 2;
    position.y = window_height - 100;

    Entity player = {
        .cleanup = cleanup,
        .handle_events = handle_events,
        .update = update,
        .render = render,
        .data = NULL  // Player speichert seine Daten in statischen Variablen
    };

    return player;
}