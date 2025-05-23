//
// Created by tb on 08.05.2025.
//

#include "health.h"
static SDL_Texture* health_texture;
static int healthpoints=3;
static SDL_FRect spritehealth;

typedef struct {
    float x, y;
} HealthData;

static void cleanup() {


}
static void handle_events(SDL_Event* event, void* data){

}
static void update(float delta_time, void* data)
 {

    switch (healthpoints) {
        case 3:
            spritehealth = (SDL_FRect){1, 3, 30, 11};
        break;
        case 2:
            spritehealth = (SDL_FRect){33, 3, 30, 11};
        break;
        case 1:
            spritehealth = (SDL_FRect){65, 3, 30, 11};
        break;
        case 0:
            spritehealth = (SDL_FRect){97, 3, 30, 11};
        break;
    }

}
static void render(SDL_Renderer* renderer, void* data) {
    SDL_FRect health_position={0,0,90,60};
    SDL_SetTextureScaleMode(health_texture, SDL_SCALEMODE_NEAREST);
    SDL_RenderTexture(renderer,health_texture, &spritehealth, &health_position);
}
Entity init_health(SDL_Renderer* renderer) {
    char pathhealth[]="C:\\Users\\tb\\CLionProjects\\Spaceinvaders\\SpaceInvaders_Health.png";
    health_texture=IMG_LoadTexture(renderer,pathhealth);
    if (!health_texture) {
        printf("Failed to load player texture: %s\n", SDL_GetError());
    }
    Entity health = {
        .cleanup = cleanup,
        .handle_events = handle_events,
        .update = update,
        .render = render,
    };
    return health;
}

