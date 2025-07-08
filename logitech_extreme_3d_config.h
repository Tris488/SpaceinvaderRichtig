// logitech_extreme_3d_config.h
// Spezifische Button-Mappings für Logitech Extreme 3D Pro

#ifndef LOGITECH_EXTREME_3D_CONFIG_H
#define LOGITECH_EXTREME_3D_CONFIG_H

#include "joystick_manager.h"

// Logitech Extreme 3D Pro Button-Layout:
// Der Joystick hat 12 Buttons und einen 8-Wege Hat-Switch
// Buttons am Stick:
//   - Trigger (Button 0)
//   - Daumen-Button (Button 1)
//   - Buttons 3-6 oben am Stick
// Buttons an der Basis:
//   - Buttons 7-12 (6 Buttons an der Basis)

// Button-Definitionen für Logitech Extreme 3D Pro
typedef enum {
    // Stick-Buttons
    L3D_TRIGGER = 0,           // Hauptabzug - Primärfeuer
    L3D_THUMB_BUTTON = 1,      // Daumen-Button - Sekundärfeuer/Bombe
    L3D_TOP_LEFT = 2,          // Oberer linker Button
    L3D_TOP_RIGHT = 3,         // Oberer rechter Button
    L3D_FRONT_LEFT = 4,        // Vorderer linker Button
    L3D_FRONT_RIGHT = 5,       // Vorderer rechter Button

    // Basis-Buttons (links)
    L3D_BASE_7 = 6,            // Button 7 auf der Basis
    L3D_BASE_8 = 7,            // Button 8 auf der Basis
    L3D_BASE_9 = 8,            // Button 9 auf der Basis

    // Basis-Buttons (rechts)
    L3D_BASE_10 = 9,           // Button 10 auf der Basis
    L3D_BASE_11 = 10,          // Button 11 auf der Basis
    L3D_BASE_12 = 11,          // Button 12 auf der Basis
} LogitechExtreme3DButton;

// Achsen für Logitech Extreme 3D Pro
typedef enum {
    L3D_AXIS_X = 0,            // Links/Rechts
    L3D_AXIS_Y = 1,            // Vor/Zurück
    L3D_AXIS_TWIST = 2,        // Drehung (Rudder)
    L3D_AXIS_THROTTLE = 3,     // Schubregler
} LogitechExtreme3DAxis;

// Hat-Switch Positionen (POV)
typedef enum {
    L3D_HAT_UP = 0,
    L3D_HAT_UP_RIGHT = 45,
    L3D_HAT_RIGHT = 90,
    L3D_HAT_DOWN_RIGHT = 135,
    L3D_HAT_DOWN = 180,
    L3D_HAT_DOWN_LEFT = 225,
    L3D_HAT_LEFT = 270,
    L3D_HAT_UP_LEFT = 315,
    L3D_HAT_CENTERED = -1
} LogitechExtreme3DHat;

// Empfohlene Button-Mappings für Ihr Space Invaders Spiel
typedef struct {
    // Kampf-Funktionen
    LogitechExtreme3DButton fire_button;        // Hauptfeuer
    LogitechExtreme3DButton bomb_button;        // Bombe
    LogitechExtreme3DButton special_button;     // Spezial-Angriff

    // Menü-Navigation
    LogitechExtreme3DButton menu_select;        // Auswahl/Bestätigen
    LogitechExtreme3DButton menu_back;          // Zurück/Abbrechen
    LogitechExtreme3DButton pause_button;       // Pause

    // Game Over Screen
    LogitechExtreme3DButton restart_button;     // Neustart
    LogitechExtreme3DButton highscore_confirm;  // Highscore bestätigen
} GameButtonMapping;

// Standard-Konfiguration für Space Invaders
static const GameButtonMapping DEFAULT_MAPPING = {
    .fire_button = L3D_TRIGGER,           // Trigger zum Schießen
    .bomb_button = L3D_THUMB_BUTTON,      // Daumen-Button für Bomben
    .special_button = L3D_TOP_RIGHT,      // Top-Right für Spezial

    .menu_select = L3D_TRIGGER,           // Trigger auch für Menü
    .menu_back = L3D_TOP_LEFT,            // Top-Left für Zurück
    .pause_button = L3D_BASE_9,           // Button 9 für Pause

    .restart_button = L3D_TRIGGER,        // Trigger für Neustart
    .highscore_confirm = L3D_BASE_12      // Button 12 für Highscore
};

// Hilfsfunktionen für Hat-Switch
static inline bool is_hat_pressed(SDL_Event* event, LogitechExtreme3DHat direction) {
    if (event->type == SDL_EVENT_JOYSTICK_HAT_MOTION) {
        return event->jhat.value == direction;
    }
    return false;
}

// Debug-Funktion für Button-Namen
static inline const char* get_l3d_button_name(LogitechExtreme3DButton button) {
    switch(button) {
        case L3D_TRIGGER: return "Trigger";
        case L3D_THUMB_BUTTON: return "Daumen-Button";
        case L3D_TOP_LEFT: return "Top-Links (3)";
        case L3D_TOP_RIGHT: return "Top-Rechts (4)";
        case L3D_FRONT_LEFT: return "Front-Links (5)";
        case L3D_FRONT_RIGHT: return "Front-Rechts (6)";
        case L3D_BASE_7: return "Basis-7";
        case L3D_BASE_8: return "Basis-8";
        case L3D_BASE_9: return "Basis-9";
        case L3D_BASE_10: return "Basis-10";
        case L3D_BASE_11: return "Basis-11";
        case L3D_BASE_12: return "Basis-12";
        default: return "Unbekannt";
    }
}

#endif // LOGITECH_EXTREME_3D_CONFIG_H