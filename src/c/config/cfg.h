#pragma once

#define SETTINGS_KEY 1

typedef enum {
    KeyFlag,

    FlagKeyCount
} FlagCfgKey;

void load_settings();
void save_settings();