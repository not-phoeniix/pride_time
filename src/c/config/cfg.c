#include <pebble.h>
#include "cfg.h"
#include "../main.h"

extern ClaySettings settings;

int flag_num;

static void set_defaults() {
    flag_num = 3;
}

void load_settings() {
    set_defaults();
    //persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
    flag_num = persist_read_int(MESSAGE_KEY_FlagKey);
}

void save_settings() {
    //persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
    persist_write_int(MESSAGE_KEY_FlagKey, flag_num);
}