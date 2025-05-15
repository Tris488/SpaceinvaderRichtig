cmake_minimum_required(VERSION 3.30)
project(Spaceinvaders C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
# Create your game executable target as usual
add_executable(Spaceinvaders WIN32 gamewindow/init.c
        gamewindow/init.h
        gamewindow/iterate.c
        gamewindow/iterate.h
        gamewindow/update.c
        gamewindow/update.h
        gamewindow/render.c
        gamewindow/render.h
        gamewindow/events.c
        gamewindow/quit.c
        gamewindow/quit.h
        gamewindow/wichitg.h
        entity.c
        entity.h
        Background.c
        Background.h
        player.c
        player.h
        health.c
        health.h
        pew.c
        pew.h
)

# Link to the actual SDL3 library.
target_link_libraries(Spaceinvaders PRIVATE SDL3::SDL3)
target_link_libraries(Spaceinvaders PRIVATE SDL3_image::SDL3_image SDL3::SDL3)

