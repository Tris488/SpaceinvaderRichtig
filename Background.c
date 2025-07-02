#include "Background.h"
static SDL_Texture* map_texture;



static void cleanup(){

}
static void handle_events(SDL_Event* event, void* data)
{

}

static void update(float delta_time, void *data) {

}

static void render(SDL_Renderer* renderer, void *data) {
    int window_width, window_height;

    SDL_GetCurrentRenderOutputSize(renderer, &window_width, &window_height);


    SDL_FRect map_position = {0, 0, (float)window_width, (float)window_height};
    SDL_SetTextureScaleMode(map_texture, SDL_SCALEMODE_NEAREST);
    SDL_RenderTexture(renderer,map_texture,NULL, &map_position);

}
Entity init_map(SDL_Renderer* renderer) {
    printf("init_map wird aufgerufen\n");

    map_texture = IMG_LoadTexture(renderer,"pictures\\SpaceInvaders_Background.bmp");

    Entity map={
        .cleanup = cleanup,
    .handle_events = handle_events,
    .update = update,
    .render = render
    };

    return map;

}