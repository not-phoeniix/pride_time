#include <pebble.h>
#include "main.h"
#include "messaging/msg.h"
#include "config/cfg.h"

static Window *main_window;
static Layer *flag, *bat_indicator;
static TextLayer *main_time, *time_bg, *main_date, *date_bg;

extern int *flag_colors[];
extern int num_stripes[];

ClaySettings settings;

static int battery_level;
int date_bool_offset;

static void bluetooth_callback(bool connected) {
    if(settings.doBtBuzz == true && !connected) {
        vibes_short_pulse();
    }
}

static void battery_callback(BatteryChargeState state) {
    battery_level = state.charge_percent;
    layer_mark_dirty(bat_indicator);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
    int h = bounds.size.h;
    int w = bounds.size.w;

    int bar_w = (battery_level * 90) / 100;
    int bar_h = 5;
    graphics_context_set_fill_color(ctx, settings.accColor);

    if(settings.BottomShadow == true) {
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) + settings.spacing, h / 2 + (25 - date_bool_offset) + settings.spacing, bar_w, bar_h), 0, GCornerNone);
    } else {
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) + settings.spacing, h / 2 + (25 - date_bool_offset) - settings.spacing, bar_w, bar_h), 0, GCornerNone);
    }

    graphics_context_set_fill_color(ctx, settings.mainColor);

    if(settings.BottomShadow == true) {
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) - settings.spacing, h / 2 + (25 - date_bool_offset) - settings.spacing, bar_w, bar_h), 0, GCornerNone);
    } else {
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) - settings.spacing, h / 2 + (25 - date_bool_offset) + settings.spacing, bar_w, bar_h), 0, GCornerNone);
    }
}

void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char t_buffer[] = "00:00";
    static char d_buffer[] = "MM-DD";

    strftime(t_buffer, sizeof(t_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    strftime(d_buffer, sizeof(d_buffer), "%m-%d", tick_time);

    text_layer_set_text(main_time, t_buffer);
    text_layer_set_text(time_bg, t_buffer);
    text_layer_set_text(main_date, d_buffer);
    text_layer_set_text(date_bg, d_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void draw_flag(int segments, int colors[], GContext *ctx) {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
    int h = bounds.size.h / segments + (bounds.size.h % segments != 0);
    int w = bounds.size.w;

    for (int i = 0; i < segments; i++) {
        GRect flag_stripe = GRect(0, h * i, w, h);

        graphics_context_set_fill_color(ctx, GColorFromHEX(colors[i]));
        graphics_fill_rect(ctx, flag_stripe, 0, GCornerNone);
    }
}

static void flag_update_proc(Layer *layer, GContext *ctx) {
    draw_flag(num_stripes[settings.flag_number], flag_colors[settings.flag_number], ctx);
}

void update_flag() {
    layer_mark_dirty(flag);
}

static void draw_time() {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));

    //y offset for date formatting
    int time_y_offset;
    if (settings.timeFant == fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49)) {
        time_y_offset = 31 + date_bool_offset;
    } else {
        time_y_offset = 26 + date_bool_offset;
    }

    //main time
    if(settings.BottomShadow == true) {
        main_time = text_layer_create(GRect(0 - settings.spacing, (bounds.size.h / 2 - time_y_offset) - settings.spacing, bounds.size.w, 50));
    } else {
        main_time = text_layer_create(GRect(0 - settings.spacing, (bounds.size.h / 2 - time_y_offset) + settings.spacing, bounds.size.w, 50));
    }

    //time bg
    if(settings.BottomShadow == true) {
        time_bg = text_layer_create(GRect(settings.spacing, (bounds.size.h / 2 - time_y_offset) + settings.spacing, bounds.size.w, 50));
    } else {
        time_bg = text_layer_create(GRect(settings.spacing, (bounds.size.h / 2 - time_y_offset) - settings.spacing, bounds.size.w, 50));
    }
}

static void draw_date() {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
    
    //y offsets for date formatting
    int date_y_offset;
    if(settings.dateFant == fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD)) {
        date_y_offset = 32 - date_bool_offset;
    } else if (settings.dateFant == fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS)) {
        date_y_offset = 36 - date_bool_offset;
    } else {
        date_y_offset = 35 - date_bool_offset;
    }

    //main date
    if(settings.BottomShadow == true) {
        main_date = text_layer_create(GRect(0 - settings.spacing, (bounds.size.h / 2 + date_y_offset) - settings.spacing, bounds.size.w, 50));
    } else {
        main_date = text_layer_create(GRect(0 - settings.spacing, (bounds.size.h / 2 + date_y_offset) + settings.spacing, bounds.size.w, 50));
    }

    //date bg
    if(settings.BottomShadow == true) {
        date_bg = text_layer_create(GRect(settings.spacing, (bounds.size.h / 2 + date_y_offset) + settings.spacing, bounds.size.w, 50));
    } else {
        date_bg = text_layer_create(GRect(settings.spacing, (bounds.size.h / 2 + date_y_offset) - settings.spacing, bounds.size.w, 50));
    }

    layer_set_hidden(text_layer_get_layer(main_date), !settings.doDate);
    layer_set_hidden(text_layer_get_layer(date_bg), !settings.doDate);
}

void update_stuff() {
    window_set_background_color(main_window, settings.bgColor);

    text_layer_set_text_color(main_time, settings.mainColor);
    text_layer_set_text_color(time_bg, settings.accColor);

    text_layer_set_text_color(main_date, settings.mainColor);
    text_layer_set_text_color(date_bg, settings.accColor);

    text_layer_set_font(main_time, settings.timeFant);
    text_layer_set_font(time_bg, settings.timeFant);

    text_layer_set_font(main_date, settings.dateFant);
    text_layer_set_font(date_bg, settings.dateFant);

    layer_set_hidden(bat_indicator, settings.hideBatBar);
    layer_mark_dirty(bat_indicator);

    update_flag();
    update_time();

    draw_time();
    draw_date();
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    window_set_background_color(main_window, settings.bgColor);

    if(settings.doDate == true) {
        date_bool_offset = 20;
    } else {
        date_bool_offset = 0;
    }

    //flag
    flag = layer_create(bounds);
    layer_set_update_proc(flag, flag_update_proc);
    layer_add_child(window_layer, flag);

    //draw date and time
    draw_time();
    draw_date();

    //main text
    text_layer_set_background_color(main_time, GColorClear);
    text_layer_set_text_color(main_time, settings.mainColor);
    text_layer_set_font(main_time, settings.timeFant);
    text_layer_set_text_alignment(main_time, GTextAlignmentCenter);

    //text drop shadow
    text_layer_set_background_color(time_bg, GColorClear);
    text_layer_set_text_color(time_bg, settings.accColor);
    text_layer_set_font(time_bg, settings.timeFant);
    text_layer_set_text_alignment(time_bg, GTextAlignmentCenter);

    //main date
    text_layer_set_font(main_date, settings.dateFant);
    text_layer_set_text_color(main_date, settings.mainColor);
    text_layer_set_background_color(main_date, GColorClear);
    text_layer_set_text_alignment(main_date, GTextAlignmentCenter);

    //bg date
    text_layer_set_font(date_bg, settings.dateFant);
    text_layer_set_text_color(date_bg, settings.accColor);
    text_layer_set_background_color(date_bg, GColorClear);
    text_layer_set_text_alignment(date_bg, GTextAlignmentCenter);

    //set childs of the layers
    layer_add_child(window_layer, text_layer_get_layer(time_bg));
    layer_add_child(window_layer, text_layer_get_layer(main_time));
    layer_add_child(window_layer, text_layer_get_layer(date_bg));
    layer_add_child(window_layer, text_layer_get_layer(main_date));

    //battery indicator
    bat_indicator = layer_create(bounds);
    layer_set_update_proc(bat_indicator, battery_update_proc);
    layer_set_hidden(bat_indicator, !settings.hideBatBar);
    layer_add_child(window_layer, bat_indicator);
}

static void main_window_unload(Window *window) {
    layer_destroy(flag);
    layer_destroy(bat_indicator);
    text_layer_destroy(main_time);
    text_layer_destroy(time_bg);
    text_layer_destroy(main_date);
    text_layer_destroy(date_bg);
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