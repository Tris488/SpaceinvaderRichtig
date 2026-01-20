# 🚀 Space Invaders

Ein klassisches Space Invaders Spiel, entwickelt in C mit SDL3.

## Beschreibung

Dieses Projekt ist ein Retro-Arcade-Shooter im Stil des Klassikers Space Invaders. Der Spieler steuert ein Raumschiff und muss sich gegen Wellen von Gegnern verteidigen. Das Spiel bietet verschiedene Gegnertypen, Power-Ups und Controller-Unterstützung.

## Features

- **Klassisches Gameplay** - Wellen-basiertes Gegner-System mit steigender Schwierigkeit
- **8 verschiedene Gegnertypen** - Kleine, mittlere und große Gegner mit unterschiedlichen Lebenspunkten
- **Power-Up-System** - Double Shoot, Speed Boost, Punktemultiplikator und Bomben
- **Controller-Support** - Volle Unterstützung für Joysticks und Gamepads
- **Retro-Grafik** - Pixel-Art Sprites mit Nearest-Neighbor-Skalierung
- **Punkte-System** - Highscore-Tracking mit MySQL-Datenbank

## Steuerung

### Tastatur
| Taste | Aktion |
|-------|--------|
| W / ↑ | Nach oben bewegen |
| S / ↓ | Nach unten bewegen |
| A / ← | Nach links bewegen |
| D / → | Nach rechts bewegen |
| Leertaste | Schießen |
| E | Bombe einsetzen |

### Controller
| Button | Aktion |
|--------|--------|
| Linker Stick | Bewegung |
| A-Button | Schießen |
| B-Button | Bombe einsetzen |

## Gegnertypen

### Kleine Gegner (1 Leben)
- **Rot** - 10 Punkte, langsam
- **Gelb** - 15 Punkte, mittel
- **Pink** - 20 Punkte, schnell
- **Blau** - 25 Punkte, mittel

### Mittlere Gegner (2 Leben)
- **Gold** - 50 Punkte
- **Dunkelgrün** - 50 Punkte
- **Silber** - 50 Punkte

### Große Gegner (3 Leben)
- **Grün** - 100 Punkte

## Power-Ups

| Power-Up | Effekt | Dauer |
|----------|--------|-------|
| Double Shoot | Zwei Schüsse gleichzeitig | Zeitbasiert |
| Speed | Erhöhte Bewegungsgeschwindigkeit | Zeitbasiert |
| Multiplier | Doppelte Punktzahl | Zeitbasiert |
| Bombe | Zerstört alle Gegner auf dem Bildschirm | Einmalig |
| Herz | +1 Leben | Sofort |

## Projektstruktur

```
Spaceinvaders/
├── mainfolder/           # Hauptspiel-Loop (Init, Update, Render, Events)
│   ├── init.c/h          # SDL-Initialisierung und Entity-Setup
│   ├── iterate.c/h       # Hauptschleife
│   ├── update.c/h        # Spiellogik-Update
│   ├── render.c/h        # Rendering
│   ├── events.c          # Event-Handling
│   ├── quit.c/h          # Cleanup
│   └── wichitg.h         # Globale Definitionen
├── player.c/h            # Spieler-Logik und Steuerung
├── enemy.c/h             # Gegner-System und Wellen
├── pew.c/h               # Schuss-System
├── health.c/h            # Leben-System
├── powerup.c/h           # Power-Up-System
├── gameover.c/h          # Game Over Screen
├── entity.c/h            # Entity-Basisklasse
├── Background.c/h        # Hintergrund
├── button.c/h            # UI-Buttons
├── joystick_manager.c/h  # Controller-Verwaltung
├── database.c/h          # MySQL-Anbindung
├── game_scale.c/h        # Bildschirm-Skalierung
├── pictures/             # Sprites und Grafiken
├── vendored/             # SDL3 und SDL3_image
└── CMakeLists.txt        # Build-Konfiguration
```

## Systemanforderungen

- Windows 10/11 (64-bit)
- Visual Studio 2022 mit C-Compiler
- CMake 3.30 oder höher
- MySQL Server (für Highscores)

## Build-Anleitung

### Voraussetzungen installieren

1. **Visual Studio 2022** mit "Desktop-Entwicklung mit C++" installieren
2. **CMake** von cmake.org installieren
3. **vcpkg** einrichten (optional, für zusätzliche Abhängigkeiten)

### Projekt bauen

```bash
# Repository klonen
git clone https://github.com/dein-username/spaceinvaders.git
cd spaceinvaders

# Build-Verzeichnis erstellen
mkdir build
cd build

# CMake konfigurieren
cmake ..

# Kompilieren
cmake --build . --config Debug
```

### MySQL-Connector einrichten

1. MySQL Connector/C herunterladen
2. In den Ordner `mysql-connector/` im Projektverzeichnis kopieren
3. Die `libmysql.dll` wird automatisch ins Build-Verzeichnis kopiert

## Abhängigkeiten

| Bibliothek | Version | Verwendung |
|------------|---------|------------|
| SDL3 | 3.x | Grafik, Input, Audio |
| SDL3_image | 3.x | Bildladen (PNG) |
| MySQL Connector/C | 8.x | Datenbank-Anbindung |

Die SDL-Bibliotheken sind als vendored Submodule im Projekt enthalten.

## Entity-System

Das Spiel verwendet ein einfaches Entity-Component-System:

```c
typedef struct {
    void (*cleanup)(void* data);
    void (*handle_events)(SDL_Event* event, void* data);
    void (*update)(float delta_time, void* data);
    void (*render)(SDL_Renderer* renderer, void* data);
    void* data;
} Entity;
```

Jede Spielkomponente (Spieler, Gegner, Schüsse, Power-Ups) ist eine Entity mit eigenen Callbacks.

## Bekannte Einschränkungen

- Der Button-Code ist aktuell auskommentiert
- Bomben-Logik ist teilweise noch nicht implementiert
- Game Over Screen zeigt nur Debug-Text

## Lizenz

Dieses Projekt ist für Ausbildungszwecke erstellt worden.

## Autor

Erstellt im Rahmen des ersten Lehrjahrs der Ausbildung zum Fachinformatiker für Anwendungsentwicklung.

---

*Letzte Aktualisierung: Januar 2026*
