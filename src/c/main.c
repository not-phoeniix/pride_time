#include <pebble.h>
#include "main.h"
#include "messaging/msg.h"
#include "config/cfg.h"
#include "drawing/drawing.h"

Window *main_window;
static Layer *flag, *bat_indicator, *time_layer, *date_layer;

ClaySettings settings;

int battery_level;
int date_bool_offset;
int no_bat_offset;

static void bluetooth_callback(bool connected) {
    if(settings.doBtBuzz == true && !connected) {
        vibes_short_pulse();
    }
}

static void battery_callback(BatteryChargeState state) {
    battery_level = state.charge_percent;
    layer_mark_dirty(bat_indicator);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    layer_mark_dirty(time_layer);
}

void formatting() {

    //text formatting n stuff
    if(settings.doDate == true) {
        date_bool_offset = 15;
        if(settings.showBatBar == false) {
            no_bat_offset = 8;
        } else {
            no_bat_offset = 0;
        }
    } else {
        date_bool_offset = 0;
        no_bat_offset = 0;
    }
}

void update_stuff() {
    window_set_background_color(main_window, settings.bgColor);

    formatting();

    layer_set_hidden(bat_indicator, !settings.showBatBar);
    layer_set_hidden(date_layer, !settings.doDate);
    
    update_time();

    layer_mark_dirty(bat_indicator);
    layer_mark_dirty(date_layer);
    layer_mark_dirty(time_layer);
    layer_mark_dirty(flag);
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    window_set_background_color(main_window, settings.bgColor);

    formatting();

    //flag
    flag = layer_create(bounds);
    layer_set_update_proc(flag, flag_update_proc);
    layer_add_child(window_layer, flag);

    //draw time
    time_layer = layer_create(bounds);
    layer_set_update_proc(time_layer, time_draw_update_proc);
    layer_add_child(window_layer, time_layer);

    //draw date
    date_layer = layer_create(bounds);
    layer_set_update_proc(date_layer, date_draw_update_proc);
    layer_set_hidden(date_layer, !settings.doDate);
    layer_add_child(window_layer, date_layer);

    //battery indicator
    bat_indicator = layer_create(bounds);
    layer_set_update_proc(bat_indicator, battery_update_proc);
    layer_set_hidden(bat_indicator, !settings.showBatBar);
    layer_add_child(window_layer, bat_indicator);
}

static void main_window_unload(Window *window) {
    layer_destroy(flag);
    layer_destroy(bat_indicator);
    layer_destroy(time_layer);
    layer_destroy(date_layer);
}

static void init() {
    main_window = window_create();

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    battery_state_service_subscribe(battery_callback);
    
    connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = bluetooth_callback
    });

    init_msg();
    load_settings();

    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(main_window, true);
    battery_callback(battery_state_service_peek());
    bluetooth_callback(connection_service_peek_pebble_app_connection());
    update_stuff();
}

static void deinit() {
    window_destroy(main_window);
    tick_timer_service_unsubscribe();
}

int main() {
    init();
    app_event_loop();
    deinit();
}