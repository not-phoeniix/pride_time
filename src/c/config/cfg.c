#include <pebble.h>
#include "cfg.h"
#include "../main.h"

extern ClaySettings settings;

static void set_defaults() {
    settings.bgColor = GColorBlack;
    settings.mainColor = GColorWhite;
    settings.accColor = GColorBlack;
    settings.BottomShadow = true;
    settings.flag_number = 1;
    settings.spacing = 2;
    settings.timeFant = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
    settings.doBatBar = true;
}

void load_settings() {
    set_defaults();
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

void save_settings() {
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}