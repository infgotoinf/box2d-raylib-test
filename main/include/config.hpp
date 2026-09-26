#pragma once

#include "raylib.h"



constexpr static float WINDOW_WIDTH  =  640.0f;
constexpr static float WINDOW_HEIGHT =  640.0f;
constexpr static int FPS = 60;
constexpr static int PIXELS_PER_METER = 64;
// constexpr static int RANDOM_SPREAD = 75;
// constexpr static int RANDOM_COLOR =  (uint8_t)(GetRandomValue(0, 10) * 10 + 15);
// constexpr static int DEFAULT_COLOR =  (Color){ RANDOM_COLOR, RANDOM_COLOR, RANDOM_COLOR, 255};


// Colors
/// Entity color
constexpr static Color COLOR_ENTITY          = DARKGRAY;
/// Entity outline color (used on hovered, selected and edited entities)
constexpr static Color COLOR_ENTITY_HOVERED  = BLUE;
constexpr static Color COLOR_ENTITY_SELECTED = SKYBLUE;
constexpr static Color COLOR_ENTITY_EDITED   = DARKBLUE;
/// Window background color
constexpr static Color COLOR_BG              = LIGHTGRAY;

// GUI colors
/// GUI border color
// constexpr static Color GUI_COLOR_ACCENT               = GREEN;
// constexpr static Color GUI_COLOR_BUTTON_TEXT          = GREEN;
// constexpr static Color GUI_COLOR_BUTTON_BG            = RAYWHITE;
// constexpr static Color GUI_COLOR_BUTTON_DISABLED_TEXT = DARKGRAY;
// constexpr static Color GUI_COLOR_BUTTON_DISABLED_BG   = GRAY;
// constexpr static Color GUI_COLOR_BUTTON_HOVERED_TEXT  = LIME;
// constexpr static Color GUI_COLOR_BUTTON_HOVERED_BG    = WHITE;
// constexpr static Color GUI_COLOR_BUTTON_PUSHED_TEXT   = DARKGREEN;
// constexpr static Color GUI_COLOR_BUTTON_PUSHED_BG     = GRAY;
constexpr static int GUI_TEXT_SIZE = 24;
constexpr static int GUI_BUTTON_MARGIN = 0;

constexpr static float ENTITY_OUTLINE_SIZE =  3.0f;
constexpr static float ENTITY_TRANSFORM_SIZE =  10.0f;

constexpr static float DEFAULT_RECTANGLE_WIDTH =  125.0f;
constexpr static float DEFAULT_RECTANGLE_HEIGHT =  75.0f;
constexpr static float DEFAULT_CIRCLE_RADIUS =  50.0f;
constexpr static float RAIN_RECTANGLE_SIZE =  15.0f;
constexpr static float RAIN_CIRCLE_SIZE =  7.5f;
