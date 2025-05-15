//Player.c
#include "player.h"
#include <stdlib.h>
#include <stdio.h>

static SDL_Texture* player_texture;
static SDL_FRect spriteplayer_portion = {0, 1, 8, 8};
static SDL_Joystick* joystick = NULL;
static SDL_JoystickID joystick_id = -1;
int window_width, window_height;
static Uint32 last_shot_time = 0;
static Uint32 shot_cooldown = 200;
Position position = {0,0}; // Start at the bottom center of screen
static float move_speed = 100.0f; // Movement speed

// Function to handle joystick connection
static void connect_joystick() {
    // Count available joysticks - correct SDL3 function
    int num_joysticks = SDL_GetJoysticks;
    printf("Available joysticks: %d\n", num_joysticks);

    if (num_joysticks > 0) {
        // Open the first available joystick
        joystick = SDL_OpenJoystick(0);

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
}

static void cleanup() {
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
    // Keyboard controls
    Uint32 current_shot_time=SDL_GetTicks();
    AppState* state = (AppState*)data;
    SDL_Renderer* renderer = state->renderer;
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP]) {
        position.y -= move_speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) {
        position.y += move_speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) {
        position.x -= move_speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) {
        position.x += move_speed * delta_time;
    }
    if (keyboard_state[SDL_SCANCODE_SPACE]) {
        if (keyboard_state[SDL_SCANCODE_SPACE]) {
            if (current_shot_time>last_shot_time+shot_cooldown){
                if (entities_count < MAX_ENTITIES) {
                    // Debug-Ausgabe
                    printf("Spieler feuert Schuss ab: x=%.2f, y=%.2f\n", position.x, position.y);

                    // Schuss erstellen
                    entities[entities_count] = create_shot_entity(renderer, position.x + 10, position.y - 20);

                    // Nur erhöhen, wenn der Schuss erfolgreich erstellt wurde
                    if (entities[entities_count].data != NULL) {
                        entities_count++;
                        printf("Schuss erstellt. Neue Entitäten-Anzahl: %d\n", entities_count);
                    }
                    last_shot_time=current_shot_time;
                }
                else {
                    printf("Maximale Anzahl an Entities erreicht!\n");
                }
            }
        }
    }

    // Joystick controls
    if (joystick) {
        Sint16 x_axis = SDL_GetJoystickAxis(joystick, 0);
        Sint16 y_axis = SDL_GetJoystickAxis(joystick, 1);

        int deadzone = 8000; // Adjust deadzone as needed

        // Apply horizontal movement if beyond deadzone
        if (abs(x_axis) > deadzone) {
            float normalized_x = (float)x_axis / 32767.0f;
            position.x += normalized_x * move_speed * delta_time;
        }

        // Apply vertical movement if beyond deadzone
        if (abs(y_axis) > deadzone) {
            float normalized_y = (float)y_axis / 32767.0f;
            position.y += normalized_y * move_speed * delta_time;
        }

        // Check joystick buttons
        int num_buttons = SDL_GetNumJoystickButtons(joystick);
        for (int i = 0; i < num_buttons; i++) {
            if (SDL_GetJoystickButton(joystick, i)) {
                // Handle button press
                // For example: if (i == 0) { fire(); }
            }
        }
    }

    // Screen boundaries


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

static void render(SDL_Renderer* renderer, void* data){  // Add data parameter
    SDL_FRect player_position = {position.x, position.y, 20, 40};
    SDL_SetTextureScaleMode(player_texture, SDL_SCALEMODE_NEAREST);
    SDL_RenderTexture(renderer, player_texture, &spriteplayer_portion, &player_position);
}
Entity init_player(SDL_Renderer* renderer) {
    const char path[] = "C:\\Users\\tb\\CLionProjects\\Spaceinvaders\\pico8_invaders_sprites_LARGE.png";
    player_texture = IMG_LoadTexture(renderer, path);

    if (!player_texture) {
        printf("Failed to load player texture: %s\n", SDL_GetError());
    }

    // Try to connect any available joystick at initialization
    connect_joystick();
    SDL_GetCurrentRenderOutputSize(renderer, &window_width, &window_height);
    position.x=window_width/2;
    position.y=window_height-100;
    Entity player = {
        .cleanup = cleanup,
        .handle_events = handle_events,
        .update = update,
        .render = render,
        .data = NULL  // Make sure this is set to NULL or actual data
    };

    return player;
}