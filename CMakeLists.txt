cmake_minimum_required(VERSION 3.30)
project(SpaceinvadersRichtig1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(PICS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pictures")

set(MYSQL_CONNECTOR_DIR "${CMAKE_SOURCE_DIR}/mysql-connector")
include_directories(${MYSQL_CONNECTOR_DIR}/include)
link_directories(${MYSQL_CONNECTOR_DIR}/lib)

# This assumes the SDL source is available in vendored/SDL x
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
# Create your game executable target as usual
add_executable(SpaceinvadersRichtig1 WIN32 mainfolder/init.c
        mainfolder/init.h
        mainfolder/iterate.c
        mainfolder/iterate.h
        mainfolder/update.c
        mainfolder/update.h
        mainfolder/render.c
        mainfolder/render.h
        mainfolder/events.c
        mainfolder/quit.c
        mainfolder/quit.h
        mainfolder/wichitg.h
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
        startframe.c
        startframe.h
        button.c
        button.h
        enemy.c
        enemy.h
        enemy_entity.c
        enemy_entity.h
        gameover.c
        gameover.h
        powerup.c
        powerup.h
        joystick_manager.c
        joystick_manager.h
        logitech_extreme_3d_config.c
        logitech_extreme_3d_config.h
        database.c
        database.h
)

add_custom_command(TARGET SpaceinvadersRichtig1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_CONNECTOR_DIR}/lib/libmysql.dll"
        $<TARGET_FILE_DIR:SpaceinvadersRichtig1>
)

# Link to the actual SDL3 library.
target_link_libraries(SpaceinvadersRichtig1 PRIVATE SDL3::SDL3)
target_link_libraries(SpaceinvadersRichtig1 PRIVATE SDL3_image::SDL3_image SDL3::SDL3)
target_link_libraries(SpaceinvadersRichtig1 PRIVATE mysql)
