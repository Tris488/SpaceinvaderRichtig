// game_scale.c
#include "game_scale.h"

// Globale Variablen
float game_scale = 1.0f;
int game_offset_x = 0;
int game_offset_y = 0;
int scaled_width = BASE_WIDTH;
int scaled_height = BASE_HEIGHT;

void update_game_scale(SDL_Renderer* renderer) {
    int window_width, window_height;
    SDL_GetCurrentRenderOutputSize(renderer, &window_width, &window_height);

    // Berechne Skalierungsfaktor
    float scale_x = (float)window_width / BASE_WIDTH;
    float scale_y = (float)window_height / BASE_HEIGHT;

    // Nimm den kleineren Wert um Aspect Ratio zu erhalten
    game_scale = (scale_x < scale_y) ? scale_x : scale_y;

    // Berechne skalierte Größe
    scaled_width = BASE_WIDTH * game_scale;
    scaled_height = BASE_HEIGHT * game_scale;

    // Berechne Offset für Zentrierung
    game_offset_x = (window_width - scaled_width) / 2;
    game_offset_y = (window_height - scaled_height) / 2;
}

float scale_value(float value) {
    return value * game_scale;
}

int scale_x(int x) {
    return game_offset_x + (int)(x * game_scale);
}

int scale_y(int y) {
    return game_offset_y + (int)(y * game_scale);
}

SDL_FRect scale_rect(SDL_FRect rect) {
    SDL_FRect scaled = {
        game_offset_x + rect.x * game_scale,
        game_offset_y + rect.y * game_scale,
        rect.w * game_scale,
        rect.h * game_scale
    };
    return scaled;
}