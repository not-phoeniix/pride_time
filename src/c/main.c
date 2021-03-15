#include <pebble.h>
#include "main.h"
#include "messaging/msg.h"
#include "config/cfg.h"

static Window *main_window;
static Layer *flag;
static TextLayer *main_time, *time_bg;

extern int *flag_colors[];
extern int num_stripes[];
extern int flag_num;

ClaySettings settings;

void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    text_layer_set_text(main_time, s_buffer);
    text_layer_set_text(time_bg, s_buffer);
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

void update_stuff() {
    layer_mark_dirty(text_layer_get_layer(main_time));
    layer_mark_dirty(text_layer_get_layer(time_bg));

    window_set_background_color(main_window, settings.bgColor);

    text_layer_set_text_color(main_time, settings.mainColor);
    text_layer_set_text_color(time_bg, settings.accColor);

    text_layer_set_font(main_time, settings.timeFant);
    text_layer_set_font(time_bg, settings.timeFant);

    update_flag();
    update_time();
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    window_set_background_color(main_window, settings.bgColor);

    int time_y_offset;
    if (settings.timeFant == fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49)) {
        time_y_offset = 31;
    } else {
        time_y_offset = 26;
    }

    flag = layer_create(bounds);
    layer_set_update_proc(flag, flag_update_proc);
    layer_add_child(window_layer, flag);

    //main text
    if(settings.BottomShadow == true) {
        main_time = text_layer_create(GRect(0 - settings.spacing, (bounds.size.h / 2 - time_y_offset) - settings.spacing, bounds.size.w, 50));
    } else {
        main_time = text_layer_create(GRect(0 - settings.spacing, (bounds.size.h / 2 - time_y_offset) + settings.spacing, bounds.size.w, 50));
    }
    text_layer_set_background_color(main_time, GColorClear);
    text_layer_set_text_color(main_time, settings.mainColor);
    text_layer_set_font(main_time, settings.timeFant);
    text_layer_set_text_alignment(main_time, GTextAlignmentCenter);

    //background drop shadow
    if(settings.BottomShadow == true) {
        time_bg = text_layer_create(GRect(settings.spacing, (bounds.size.h / 2 - time_y_offset) + settings.spacing, bounds.size.w, 50));
    } else {
        time_bg = text_layer_create(GRect(settings.spacing, (bounds.size.h / 2 - time_y_offset) - settings.spacing, bounds.size.w, 50));
    }
    text_layer_set_background_color(time_bg, GColorClear);
    text_layer_set_text_color(time_bg, settings.accColor);
    text_layer_set_font(time_bg, settings.timeFant);
    text_layer_set_text_alignment(time_bg, GTextAlignmentCenter);

    layer_add_child(window_layer, text_layer_get_layer(time_bg));
    layer_add_child(window_layer, text_layer_get_layer(main_time));
}

static void main_window_unload(Window *window) {
    //layer_destroy(flag);
    text_layer_destroy(main_time);
    text_layer_destroy(time_bg);
    
}

static void init() {
    main_window = window_create();

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    init_msg();
    load_settings();

    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(main_window, true);
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