#include <pebble.h>
#include "main.h"
#include "messaging/msg.h"
#include "config/cfg.h"

static Window *main_window;
static Layer *flag;

extern int *flag_colors[];
extern int num_stripes[];
extern int flag_num;

ClaySettings settings;

static void draw_flag(int segments, int colors[], GContext *ctx) {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
    int h = bounds.size.h;
    int w = bounds.size.w;

    for (int i = 0; i < segments; i++) {
        GRect flag_stripe = GRect(0, h / segments * i, w, h / segments);

        graphics_context_set_fill_color(ctx, GColorFromHEX(colors[i]));
        graphics_fill_rect(ctx, flag_stripe, 0, GCornerNone);
    }
}

static void flag_update_proc(Layer *layer, GContext *ctx) {
    draw_flag(num_stripes[flag_num], flag_colors[flag_num], ctx);
}

void update_flag() {
    layer_mark_dirty(flag);
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    flag = layer_create(bounds);
    layer_set_update_proc(flag, flag_update_proc);
    layer_add_child(window_layer, flag);
}

static void main_window_unload(Window *window) {
    layer_destroy(flag);
}

static void init() {
    main_window = window_create();

    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    load_settings();
    init_msg();

    window_stack_push(main_window, true);
    update_flag();
}

static void deinit() {
    window_destroy(main_window);
}

int main() {
    init();
    app_event_loop();
    deinit();
}