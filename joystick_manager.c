// joystick_manager.c
#include "joystick_manager.h"
#include <stdio.h>
#include <string.h>

// Private Struktur für Joystick-Zustand
typedef struct {
    SDL_Joystick* joystick;
    SDL_JoystickID id;
    bool connected;

    // Button-Zustände
    bool button_states[BUTTON_MAX];
    bool button_previous[BUTTON_MAX];

    // Achsen-Werte
    float axis_values[AXIS_MAX];

    // Deadzone für Analogsticks
    float deadzone;
} JoystickState;

// Statische globale Instanz
static JoystickState joystick_state = {
    .joystick = NULL,
    .id = -1,
    .connected = false,
    .deadzone = 0.1f  // Standard-Deadzone von 10%
};

// Private Hilfsfunktionen
static void connect_joystick(void);
static void disconnect_joystick(void);
static float apply_deadzone(float value, float deadzone);

// Initialisierung
void joystick_manager_init(void) {
    printf("[Joystick Manager] Initialisierung...\n");

    // Reset der Zustände
    memset(joystick_state.button_states, 0, sizeof(joystick_state.button_states));
    memset(joystick_state.button_previous, 0, sizeof(joystick_state.button_previous));
    memset(joystick_state.axis_values, 0, sizeof(joystick_state.axis_values));

    // Setze eine höhere Deadzone für den Logitech Extreme 3D Pro
    joystick_state.deadzone = 0.15f;  // 15% Deadzone statt 10%

    // Versuche Joystick zu verbinden
    connect_joystick();
}

// Cleanup
void joystick_manager_cleanup(void) {
    printf("[Joystick Manager] Cleanup...\n");
    disconnect_joystick();
}

// Private Funktion: Joystick verbinden
static void connect_joystick(void) {
    // Prüfe verfügbare Joysticks
    int num_joysticks = 0;
    SDL_JoystickID *joysticks = SDL_GetJoysticks(&num_joysticks);

    printf("[Joystick Manager] Verfügbare Joysticks: %d\n", num_joysticks);

    if (num_joysticks > 0 && joysticks) {
        // Öffne den ersten verfügbaren Joystick
        joystick_state.joystick = SDL_OpenJoystick(joysticks[0]);

        if (joystick_state.joystick) {
            joystick_state.id = SDL_GetJoystickID(joystick_state.joystick);
            joystick_state.connected = true;

            printf("[Joystick Manager] Verbunden mit: %s\n",
                   SDL_GetJoystickName(joystick_state.joystick));
            printf("[Joystick Manager] ID: %d, Achsen: %d, Buttons: %d\n",
                   joystick_state.id,
                   SDL_GetNumJoystickAxes(joystick_state.joystick),
                   SDL_GetNumJoystickButtons(joystick_state.joystick));
        } else {
            printf("[Joystick Manager] Fehler beim Öffnen: %s\n", SDL_GetError());
        }
    }

    // Speicher freigeben
    if (joysticks) {
        SDL_free(joysticks);
    }
}

// Private Funktion: Joystick trennen
static void disconnect_joystick(void) {
    if (joystick_state.joystick) {
        SDL_CloseJoystick(joystick_state.joystick);
        joystick_state.joystick = NULL;
        joystick_state.id = -1;
        joystick_state.connected = false;
        printf("[Joystick Manager] Joystick getrennt\n");
    }
}

// Event-Handling
void joystick_handle_event(SDL_Event* event) {
    if (!event) return;

    switch (event->type) {
        case SDL_EVENT_JOYSTICK_ADDED:
            printf("[Joystick Manager] Joystick hinzugefügt\n");
            if (!joystick_state.connected) {
                connect_joystick();
            }
            break;

        case SDL_EVENT_JOYSTICK_REMOVED:
            if (joystick_state.connected && event->jdevice.which == joystick_state.id) {
                printf("[Joystick Manager] Aktiver Joystick entfernt\n");
                disconnect_joystick();
            }
            break;

        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
            if (joystick_state.connected && event->jbutton.which == joystick_state.id) {
                if (event->jbutton.button < BUTTON_MAX) {
                    joystick_state.button_states[event->jbutton.button] = true;
                }
            }
            break;

        case SDL_EVENT_JOYSTICK_BUTTON_UP:
            if (joystick_state.connected && event->jbutton.which == joystick_state.id) {
                if (event->jbutton.button < BUTTON_MAX) {
                    joystick_state.button_states[event->jbutton.button] = false;
                }
            }
            break;

        case SDL_EVENT_JOYSTICK_AXIS_MOTION:
            if (joystick_state.connected && event->jaxis.which == joystick_state.id) {
                if (event->jaxis.axis < AXIS_MAX) {
                    // SDL3 verwendet Sint16 für Achsenwerte (-32768 bis 32767)
                    float normalized = event->jaxis.value / 32768.0f;
                    joystick_state.axis_values[event->jaxis.axis] = normalized;
                }
            }
            break;
    }
}

// Update-Funktion (für just_pressed/released)
void joystick_update(void) {
    // Speichere vorherige Button-Zustände
    memcpy(joystick_state.button_previous, joystick_state.button_states,
           sizeof(joystick_state.button_states));
}

// Verbindungsstatus
bool joystick_is_connected(void) {
    return joystick_state.connected;
}

const char* joystick_get_name(void) {
    if (joystick_state.connected && joystick_state.joystick) {
        return SDL_GetJoystickName(joystick_state.joystick);
    }
    return "Nicht verbunden";
}

int joystick_get_id(void) {
    return joystick_state.id;
}

// Input-Abfragen
bool joystick_is_button_pressed(JoystickButton button) {
    if (!joystick_state.connected || button >= BUTTON_MAX) {
        return false;
    }
    return joystick_state.button_states[button];
}

bool joystick_is_button_just_pressed(JoystickButton button) {
    if (!joystick_state.connected || button >= BUTTON_MAX) {
        return false;
    }
    return joystick_state.button_states[button] && !joystick_state.button_previous[button];
}

bool joystick_is_button_just_released(JoystickButton button) {
    if (!joystick_state.connected || button >= BUTTON_MAX) {
        return false;
    }
    return !joystick_state.button_states[button] && joystick_state.button_previous[button];
}

float joystick_get_axis(JoystickAxis axis) {
    if (!joystick_state.connected || axis >= AXIS_MAX) {
        return 0.0f;
    }
    return joystick_state.axis_values[axis];
}

float joystick_get_axis_normalized(JoystickAxis axis) {
    if (!joystick_state.connected || axis >= AXIS_MAX) {
        return 0.0f;
    }

    float value = joystick_state.axis_values[axis];
    return apply_deadzone(value, joystick_state.deadzone);
}

// Deadzone-Hilfsfunktion
static float apply_deadzone(float value, float deadzone) {
    if (value > -deadzone && value < deadzone) {
        return 0.0f;
    }

    // Skaliere den Wert außerhalb der Deadzone
    if (value > 0) {
        return (value - deadzone) / (1.0f - deadzone);
    } else {
        return (value + deadzone) / (1.0f - deadzone);
    }
}

// Deadzone-Einstellungen
void joystick_set_deadzone(float deadzone) {
    if (deadzone >= 0.0f && deadzone < 1.0f) {
        joystick_state.deadzone = deadzone;
    }
}

float joystick_get_deadzone(void) {
    return joystick_state.deadzone;
}

// Utility-Funktionen
void joystick_print_info(void) {
    printf("\n=== Joystick Info ===\n");
    printf("Verbunden: %s\n", joystick_state.connected ? "Ja" : "Nein");

    if (joystick_state.connected) {
        printf("Name: %s\n", joystick_get_name());
        printf("ID: %d\n", joystick_state.id);
        printf("Deadzone: %.2f\n", joystick_state.deadzone);

        // Gedrückte Buttons anzeigen
        printf("Buttons: ");
        for (int i = 0; i < BUTTON_MAX; i++) {
            if (joystick_state.button_states[i]) {
                printf("%d ", i);
            }
        }
        printf("\n");

        // Achsenwerte anzeigen
        printf("Achsen:\n");
        for (int i = 0; i < AXIS_MAX && i < 6; i++) {
            float value = joystick_state.axis_values[i];
            if (value != 0.0f) {
                printf("  Achse %d: %.2f\n", i, value);
            }
        }
    }
    printf("====================\n");
}