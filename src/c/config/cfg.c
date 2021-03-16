#include <pebble.h>
#include "cfg.h"
#include "../main.h"

extern ClaySettings settings;

static void set_defaults() {
    settings.bgColor = GColorBlack;
    settings.mainColor = GColorWhite;
    settings.accColor = GColorBlack;
    settings.BottomShadow = true;
    settings.flag_number = 16;
    settings.spacing = 2;
    settings.timeFant = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
    settings.hideBatBar = false;
    settings.doBtBuzz = true;
    settings.doDate = true;
    settings.dateFant = fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS);
}

void load_settings() {
    set_defaults();
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

void save_settings() {
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}