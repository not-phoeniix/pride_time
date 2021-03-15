#include <pebble.h>
#include "msg.h"
#include "../main.h"
#include "../config/cfg.h"

extern ClaySettings settings;

static void inbox_recieved_handler(DictionaryIterator *iter, void *ctx) {
    Tuple *flag_t = dict_find(iter, MESSAGE_KEY_FlagKey);
    if (flag_t) {
        settings.flag_number = atoi(flag_t->value->cstring);
    }

    Tuple *bottomShadow_t = dict_find(iter, MESSAGE_KEY_BottomShadowKey);
    if(bottomShadow_t) {
        settings.BottomShadow = bottomShadow_t->value->int32 == 1;
    }

    Tuple *spacing_t = dict_find(iter, MESSAGE_KEY_SpacingKey);
    if(spacing_t) {
        settings.spacing = spacing_t->value->int32;
    }

    Tuple *bgColor_t = dict_find(iter, MESSAGE_KEY_BGColorKey);
    if(bgColor_t) {
        settings.bgColor = GColorFromHEX(bgColor_t->value->int32);
    }

    Tuple *mainColor_t = dict_find(iter, MESSAGE_KEY_MainColorKey);
    if(mainColor_t) {
        settings.mainColor = GColorFromHEX(mainColor_t->value->int32);
    }

    Tuple *accColor_t = dict_find(iter, MESSAGE_KEY_AccentColorKey);
    if(accColor_t) {
        settings.accColor = GColorFromHEX(accColor_t->value->int32);
    }

    Tuple *timeFont_t = dict_find(iter, MESSAGE_KEY_TimeFontKey);
    if(timeFont_t) {
        settings.timeFant = fonts_get_system_font(timeFont_t->value->cstring);
    }

    Tuple *bat_t = dict_find(iter, MESSAGE_KEY_EnableBatteryKey);
    if(bat_t) {
        settings.doBatBar = bat_t->value->int32 == 0;
    }

    Tuple *btbuzz_t = dict_find(iter, MESSAGE_KEY_EnableBTBuzzKey);
    if(btbuzz_t) {
        settings.doBtBuzz = btbuzz_t->value->int32 == 1;
    }

    save_settings();
    update_stuff();
}

void init_msg() {
    app_message_register_inbox_received(inbox_recieved_handler);
    app_message_open(256, 256);
}