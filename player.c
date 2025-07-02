//Player.c
#include "player.h"
#include <stdlib.h>
#include <stdio.h>
#include "enemy.h"
#include "health.h"
#include "gameover.h"
#include "powerup.h"
static SDL_Texture* player_texture;
static SDL_FRect spriteplayer_portion = {0, 1, 8, 8};
static SDL_Joystick* joystick = NULL;
static SDL_JoystickID joystick_id = -1;
int window_width, window_height;
static Uint32 last_shot_time = 0;
static Uint32 shot_cooldown = 900;
Position position = {0,0}; // Start at the bottom center of screen
static float move_speed = 200.0f; // Movement speed
static bool e_key_was_pressed = false;
static bool b_button_was_pressed = false;
// Function to handle joystick connection
static void connect_joystick() {
    // Count available joysticks - SDL3 Version
    int num_joysticks = 0;
    SDL_JoystickID *joysticks = SDL_GetJoysticks(&num_joysticks);

    printf("Available joysticks: %d\n", num_joysticks);

    if (num_joysticks > 0 && joysticks) {
        // Open the first available joystick
        joystick = SDL_OpenJoystick(joysticks[0]);

        if (joystick) {
            joystick_id = SDL_GetJoystickID(joystick);
            printf("Joystick connected! Name: %s, Instance ID: %d\n",
                  SDL_GetJoystickName(joystick), joystick_id);
            printf("Axes: %d, Buttons: %d\n",
                  SDL_GetNumJoystickAxes(joystick),
                  SDL_GetNumJoystickButtons(joystick));
        } else {
            printf("Failed to open joystick: %s\n", SDL_GetError());
        }
    }

    // Wichtig: Speicher freigeben
    if (joysticks) {
        SDL_free(joysticks);
    }
}

static void cleanup(void* data) {
    if (joystick) {
        SDL_CloseJoystick(joystick);
        joystick = NULL;
    }
    if (player_texture) {
        SDL_DestroyTexture(player_texture);
        player_texture = NULL;
    }
}

// Process joystick-specific events
static void handle_events(SDL_Event* event, void* data) {
    if (!event) return;

    if (event->type == SDL_EVENT_JOYSTICK_ADDED) {
        printf("Joystick connected. Attempting to open...\n");
        connect_joystick();
    }
    else if (event->type == SDL_EVENT_JOYSTICK_REMOVED) {
        if (joystick && event->jdevice.which == joystick_id) {
            printf("Active joystick disconnected\n");
            SDL_CloseJoystick(joystick);
            joystick = NULL;
            joystick_id = -1;
        }
    }
    else if (event->type == SDL_EVENT_JOYSTICK_BUTTON_DOWN) {
        printf("Joystick button pressed: %d\n", event->jbutton.button);
        // Can add specific actions for buttons here
    }
}

static void update(float delta_time, void* data) {
    Uint32 effective_cooldown;
    float effective_movespeed;
    if (is_game_over()) return;
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

    // Shooting
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
                    printf("Schuss erstellt. Neue Entitäten-Anzahl: %d\n", entities_count);
                    last_shot_time = current_shot_time;
                }
            }
            else {
                printf("Maximale Anzahl an Entities erreicht!\n");
            }
        }
    }
    if (keyboard_state[SDL_SCANCODE_E] && !e_key_was_pressed) {

        if (get_bomb_count() > 0) {
            bomb_got_used();
            destroy_all_enemies();

            printf("Bombe gezündet! Verbleibend: %d\n", get_bomb_count());
        } else {
            printf("Keine Bomben verfügbar!\n");
        }
    }

    // Joystick controls
    if (joystick) {
        Sint16 x_axis = SDL_GetJoystickAxis(joystick, 0);
        Sint16 y_axis = SDL_GetJoystickAxis(joystick, 1);

        int deadzone = 7600; // Adjust deadzone as needed

        // Apply horizontal movement if beyond deadzone
        if (abs(x_axis) > deadzone) {
            effective_movespeed=move_speed*get_speed_multplier();
            float normalized_x = (float)x_axis / 32767.0f;
            position.x += normalized_x * effective_movespeed * delta_time;
        }

        // Apply vertical movement if beyond deadzone
        if (abs(y_axis) > deadzone) {
            effective_movespeed=move_speed*get_speed_multplier();
            float normalized_y = (float)y_axis / 32767.0f;
            position.y += normalized_y * effective_movespeed * delta_time;
        }

        // Check joystick buttons for shooting
        if (SDL_GetJoystickButton(joystick, 0)) { // A button
            effective_cooldown = (Uint32)(shot_cooldown * get_double_shoot_multiplier());
            if (current_shot_time > last_shot_time + effective_cooldown) {
                if (entities_count < MAX_ENTITIES) {
                    Entity new_shot = create_shot_entity(renderer, position.x + 10, position.y - 20);
                    if (new_shot.data != NULL) {
                        entities[entities_count] = new_shot;
                        entities_count++;
                        last_shot_time = current_shot_time;
                    }
                }
            }
        }
        bool b_button_pressed = SDL_GetJoystickButton(joystick, 1);

        if (b_button_pressed && !b_button_was_pressed) {
            if (get_bomb_count() > 0) {
                bomb_got_used();
                destroy_all_enemies();
                printf("Bombe gezündet (Joystick)! Verbleibend: %d\n", get_bomb_count());
            }
        }

        b_button_was_pressed = b_button_pressed;

    }

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
    player_texture = IMG_LoadTexture(renderer, "pictures\\pico8_invaders_sprites_LARGE.png");

    if (!player_texture) {
        printf("Failed to load player texture: %s\n", SDL_GetError());
    }

    // Try to connect any available joystick at initialization
    connect_joystick();
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