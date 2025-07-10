// joystick_manager.h
#ifndef JOYSTICK_MANAGER_H
#define JOYSTICK_MANAGER_H

#include <SDL3/SDL.h>
#include <stdbool.h>

// Joystick-Button-Definitionen (anpassbar je nach Controller)
typedef enum {
    BUTTON_A = 0,
    BUTTON_B = 1,
    BUTTON_X = 2,
    BUTTON_Y = 3,
    BUTTON_LEFT_SHOULDER = 4,
    BUTTON_RIGHT_SHOULDER = 5,
    BUTTON_BACK = 6,
    BUTTON_START = 7,
    BUTTON_LEFT_STICK = 8,
    BUTTON_RIGHT_STICK = 9,
    BUTTON_DPAD_UP = 10,
    BUTTON_DPAD_DOWN = 11,
    BUTTON_DPAD_LEFT = 12,
    BUTTON_DPAD_RIGHT = 13,
    BUTTON_MAX
} JoystickButton;

// Achsen-Definitionen
typedef enum {
    AXIS_LEFT_X = 0,
    AXIS_LEFT_Y = 1,
    AXIS_RIGHT_X = 2,
    AXIS_RIGHT_Y = 3,
    AXIS_LEFT_TRIGGER = 4,
    AXIS_RIGHT_TRIGGER = 5,
    AXIS_MAX
} JoystickAxis;

// Initialisierung und Cleanup
void joystick_manager_init(void);
void joystick_manager_cleanup(void);

// Verbindungsstatus
bool joystick_is_connected(void);
const char* joystick_get_name(void);
int joystick_get_id(void);

// Event-Handling
void joystick_handle_event(SDL_Event* event);

// Input-Abfragen
bool joystick_is_button_pressed(JoystickButton button);
bool joystick_is_button_just_pressed(JoystickButton button);
bool joystick_is_button_just_released(JoystickButton button);
float joystick_get_axis(JoystickAxis axis);
float joystick_get_axis_normalized(JoystickAxis axis); // -1.0 bis 1.0

// Deadzone-Einstellungen
void joystick_set_deadzone(float deadzone);
float joystick_get_deadzone(void);

// Utility-Funktionen
void joystick_print_info(void);
void joystick_update(void); // Muss jeden Frame aufgerufen werden für just_pressed/released

#endif // JOYSTICK_MANAGER_H