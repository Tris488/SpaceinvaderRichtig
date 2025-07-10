//Player.c
#include "player.h"
#include <stdlib.h>
#include <stdio.h>
#include "enemy.h"
#include "health.h"
#include "gameover.h"
#include "powerup.h"
#include "game_scale.h"
#include "pew.h"  // Für create_shot_entity

static SDL_Texture* player_texture;
static SDL_FRect spriteplayer_portion = {0, 1, 8, 8};
static SDL_Joystick* joystick = NULL;
static SDL_JoystickID joystick_id = -1;
static Uint32 last_shot_time = 0;
static Uint32 shot_cooldown = 900;
Position position = {0,0};
static float move_speed = 200.0f;
static bool e_key_was_pressed = false;
static bool b_button_was_pressed = false;
static SDL_Renderer* saved_renderer = NULL; // NEU: Renderer speichern

// Funktion zum Erstellen eines Schusses
static void fire_shot(float x, float y) {
    if (saved_renderer && entities_count < MAX_ENTITIES) {
        Entity shot = create_shot_entity(saved_renderer, x, y);
        entities[entities_count++] = shot;

        // Double-Shot Check
        if (is_double_shoot_active() && entities_count < MAX_ENTITIES) {
            Entity shot2 = create_shot_entity(saved_renderer, x + 20, y);
            entities[entities_count++] = shot2;
        }
    }
}

// Function to handle joystick connection
static void connect_joystick() {
    int num_joysticks = 0;
    SDL_JoystickID *joysticks = SDL_GetJoysticks(&num_joysticks);

    printf("Available joysticks: %d\n", num_joysticks);

    if (num_joysticks > 0 && joysticks) {
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
    saved_renderer = NULL;
}

static void handle_events(SDL_Event* event, void* data) {
    if (!event) return;

    if (event->type == SDL_EVENT_JOYSTICK_ADDED) {
        printf("Joystick connected.\n");
        connect_joystick();
    }
    else if (event->type == SDL_EVENT_JOYSTICK_REMOVED) {
        if (event->jdevice.which == joystick_id) {
            printf("Joystick disconnected.\n");
            SDL_CloseJoystick(joystick);
            joystick = NULL;
            joystick_id = -1;
        }
    }
}

static void update(float delta_time, void* data) {
    if (!player_texture) return;

    // NEU: Keine Eingaben wenn Game Over
    if (is_game_over()) return;

    // Keyboard input
    const bool *keyboard_state = SDL_GetKeyboardState(NULL);

    if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) {
        position.x -= move_speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) {
        position.x += move_speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP]) {
        position.y -= move_speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) {
        position.y += move_speed * delta_time;
    }

    // Joystick input
    if (joystick) {
        Sint16 x_axis = SDL_GetJoystickAxis(joystick, 0);
        Sint16 y_axis = SDL_GetJoystickAxis(joystick, 1);

        const Sint16 DEAD_ZONE = 8000;

        if (abs(x_axis) > DEAD_ZONE) {
            float normalized_x = x_axis / 32767.0f;
            position.x += normalized_x * move_speed * delta_time;
        }

        if (abs(y_axis) > DEAD_ZONE) {
            float normalized_y = y_axis / 32767.0f;
            position.y += normalized_y * move_speed * delta_time;
        }

        bool a_button_pressed = SDL_GetJoystickButton(joystick, 0);
        if (a_button_pressed || keyboard_state[SDL_SCANCODE_SPACE]) {
            Uint32 current_time = SDL_GetTicks();
            if (current_time - last_shot_time >= shot_cooldown) {
                fire_shot(position.x + 10, position.y);  // NEU: Verwende fire_shot
                last_shot_time = current_time;
            }
        }
    } else if (keyboard_state[SDL_SCANCODE_SPACE]) {
        Uint32 current_time = SDL_GetTicks();
        if (current_time - last_shot_time >= shot_cooldown) {
            fire_shot(position.x + 10, position.y);  // NEU: Verwende fire_shot
            last_shot_time = current_time;
        }
    }

    // E-Taste für Bomben
    bool e_key_pressed = keyboard_state[SDL_SCANCODE_E];
    bool b_button_pressed = joystick ? SDL_GetJoystickButton(joystick, 1) : false;

    if ((e_key_pressed && !e_key_was_pressed) || (b_button_pressed && !b_button_was_pressed)) {
        if (get_bomb_count() > 0) {
            // use_bomb(); // Noch nicht implementiert
            destroy_all_enemies();
            printf("BOOM! Alle Gegner zerstört! Bomben verbleibend: %d\n", get_bomb_count() - 1);
        }
    }
    e_key_was_pressed = e_key_pressed;
    b_button_was_pressed = b_button_pressed;

    // Kollisionen
    SDL_FRect player_rect = {position.x, position.y, 20, 40};
    Enemy* colliding_enemy = collision_with_player(&player_rect);
    Powerup* collected = powerup_check_collision(&player_rect);

    if (collected) {
        printf("Power-Up collected!\n");
    }

    if (colliding_enemy) {
        damage_player(1);
        enemy_destroy(colliding_enemy);

        static Uint32 last_hit_time = 0;
        Uint32 current_time = SDL_GetTicks();

        if (current_time - last_hit_time > 1000) {
            last_hit_time = current_time;
        }
    }

    // Spieler innerhalb BASE_WIDTH/BASE_HEIGHT halten
    if (position.x < 0) {
        position.x = 0;
    }
    if (position.x > BASE_WIDTH - 20) {
        position.x = BASE_WIDTH - 20;
    }
    if (position.y < 60) {
        position.y = 60;
    }
    if (position.y > BASE_HEIGHT - 40) {
        position.y = BASE_HEIGHT - 40;
    }
}

static void render(SDL_Renderer* renderer, void* data) {
    if (!renderer || !player_texture) return;

    SDL_FRect player_position = {position.x, position.y, 20, 40};
    SDL_SetTextureScaleMode(player_texture, SDL_SCALEMODE_NEAREST);
    SDL_RenderTexture(renderer, player_texture, &spriteplayer_portion, &player_position);
}

Entity init_player(SDL_Renderer* renderer) {
    saved_renderer = renderer;  // NEU: Renderer speichern

    player_texture = IMG_LoadTexture(renderer, "pictures\\pico8_invaders_sprites_LARGE.png");

    if (!player_texture) {
        printf("Failed to load player texture: %s\n", SDL_GetError());
    }

    connect_joystick();

    // Startposition basiert auf BASE_WIDTH/BASE_HEIGHT
    position.x = BASE_WIDTH / 2;
    position.y = BASE_HEIGHT - 100;

    Entity player = {
        .cleanup = cleanup,
        .handle_events = handle_events,
        .update = update,
        .render = render,
        .data = NULL
    };

    return player;
}