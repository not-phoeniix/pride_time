#pragma once
#include <pebble.h>

typedef struct ClaySettings {
    int flag_number;
    bool BottomShadow;
    int spacing;
    GColor bgColor;
    GColor mainColor;
    GColor accColor;
    GFont timeFant;
    GFont dateFant;
    bool hideBatBar;
    bool doBtBuzz;
    bool doDate;
} ClaySettings;

void update_stuff();