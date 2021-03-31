#include <pebble.h>
#include <pebble-effect-layer/pebble-effect-layer.h>
#include "main.h"
#include "messaging/msg.h"
#include "config/cfg.h"

static Window *main_window;
static Layer *flag, *bat_indicator, *time_layer, *date_layer;
static EffectLayer *effect_layer;
static EffectMask *bgMask, *textMask;

static char time_char[] = "00:00";
static char date_char[] = "MM-DD";

extern int *flag_colors[];
extern int num_stripes[];

ClaySettings settings;

static int battery_level;
static int date_bool_offset;
static int no_bat_offset;
static float flag_stripe_scale;
static int flag_stripe_offset;

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
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) + settings.spacing, h / 2 + (28 - date_bool_offset) + settings.spacing, bar_w, bar_h), 0, GCornerNone);
    } else {
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) + settings.spacing, h / 2 + (28 - date_bool_offset) - settings.spacing, bar_w, bar_h), 0, GCornerNone);
    }

    graphics_context_set_fill_color(ctx, settings.mainColor);

    if(settings.BottomShadow == true) {
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) - settings.spacing, h / 2 + (28 - date_bool_offset) - settings.spacing, bar_w, bar_h), 0, GCornerNone);
    } else {
        graphics_fill_rect(ctx, GRect((w / 2 - bar_w / 2) - settings.spacing, h / 2 + (28 - date_bool_offset) + settings.spacing, bar_w, bar_h), 0, GCornerNone);
    }
}

static void formatting() {

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

    //flag formatting
    if(settings.toggleTextFlagMask) {
        flag_stripe_scale = 0.6;
        flag_stripe_offset = 20;
    } else {
        flag_stripe_offset = 0;
        flag_stripe_scale = 1;
    }
}

void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char m_buffer[3];
    static char d_buffer[3];

    strftime(time_char, sizeof(time_char), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    strftime(m_buffer, sizeof(m_buffer), "%m", tick_time);
    strftime(d_buffer, sizeof(d_buffer), "%d", tick_time);

    if(settings.switchDate) {
        snprintf(date_char, sizeof(date_char), "%s%c%s", d_buffer, settings.dateSeparator, m_buffer);
    } else {
        snprintf(date_char, sizeof(date_char), "%s%c%s", m_buffer, settings.dateSeparator, d_buffer);
    }

    APP_LOG(APP_LOG_LEVEL_INFO, "%s", date_char);

    layer_mark_dirty(time_layer);
    layer_mark_dirty(date_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void draw_flag(int segments, int colors[], float sy, int y, GContext *ctx) {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));

    int h = bounds.size.h / segments + (bounds.size.h % segments != 0);
    int w = bounds.size.w;

    for (int i = 0; i < segments; i++) {
        GRect flag_stripe = GRect(0, h * i * sy + y, w, h * sy);

        graphics_context_set_fill_color(ctx, GColorFromHEX(colors[i]));
        graphics_fill_rect(ctx, flag_stripe, 0, GCornerNone);
    }
}

static void flag_update_proc(Layer *layer, GContext *ctx) {
    draw_flag(num_stripes[settings.flag_number], flag_colors[settings.flag_number], flag_stripe_scale, flag_stripe_offset, ctx);
}

void update_flag() {
    layer_mark_dirty(flag);
}

static void draw_time(GContext *ctx) {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));

    //y offset for date formatting
    int time_y_offset;
    if (settings.timeFant == fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49)) {
        time_y_offset = 31 + date_bool_offset - no_bat_offset;
    } else {
        time_y_offset = 26 + date_bool_offset - no_bat_offset;
    }

    formatting();

    //time bg
    graphics_context_set_text_color(ctx, settings.accColor);
    if(settings.BottomShadow == true) {
        graphics_draw_text(ctx, time_char, settings.timeFant, GRect(settings.spacing, (bounds.size.h / 2 - time_y_offset) + settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
    } else {
        graphics_draw_text(ctx, time_char, settings.timeFant, GRect(settings.spacing, (bounds.size.h / 2 - time_y_offset) - settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
    }

    //main time
    graphics_context_set_text_color(ctx, settings.mainColor);
    if(settings.BottomShadow == true) {
        graphics_draw_text(ctx, time_char, settings.timeFant, GRect(0 - settings.spacing, (bounds.size.h / 2 - time_y_offset) - settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
    } else {
        graphics_draw_text(ctx, time_char, settings.timeFant, GRect(0 - settings.spacing, (bounds.size.h / 2 - time_y_offset) + settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0); 
    }
}

static void draw_date(GContext *ctx) {
    GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
    
    //y offsets for date formatting
    int date_y_offset;
    if(settings.dateFant == fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD)) {
        date_y_offset = 32 - date_bool_offset - no_bat_offset;
    } else if (settings.dateFant == fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS)) {
        date_y_offset = 36 - date_bool_offset - no_bat_offset;
    } else {
        date_y_offset = 35 - date_bool_offset - no_bat_offset;
    }

    //main date
    if(settings.doDate == false) {
        graphics_context_set_text_color(ctx, GColorClear);
    } else {
        graphics_context_set_text_color(ctx, settings.accColor);
    }
    if(settings.BottomShadow == true) {
        graphics_draw_text(ctx, date_char, settings.dateFant, GRect(0.5 * settings.spacing, (bounds.size.h / 2 + date_y_offset) + 0.5 * settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
    } else {
        graphics_draw_text(ctx, date_char, settings.dateFant, GRect(0.5 * settings.spacing, (bounds.size.h / 2 + date_y_offset) - 0.5 * settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
    }

    //date bg
    if(settings.doDate == false) {
        graphics_context_set_text_color(ctx, GColorClear);
    } else {
        graphics_context_set_text_color(ctx, settings.mainColor);
    }
    if(settings.BottomShadow == true) {
        graphics_draw_text(ctx, date_char, settings.dateFant, GRect(0 - 0.5 * settings.spacing, (bounds.size.h / 2 + date_y_offset) - 0.5 * settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
    } else {
        graphics_draw_text(ctx, date_char, settings.dateFant, GRect(0 - 0.5 * settings.spacing, (bounds.size.h / 2 + date_y_offset) + 0.5 * settings.spacing, bounds.size.w, 50), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
    }

    //layer_set_hidden(text_layer_get_layer(main_date), !settings.doDate);
    //layer_set_hidden(text_layer_get_layer(date_bg), !settings.doDate);
}

static void draw_bg_mask(GContext *ctx) {

}

static void time_draw_update_proc(Layer *layer, GContext *ctx) {
    draw_time(ctx);
}

static void date_draw_update_proc(Layer *layer, GContext *ctx) {
    draw_date(ctx);
}

static void bg_mask_shape_update_proc(Layer *layer, GContext *ctx) {

}

void update_stuff() {
    window_set_background_color(main_window, settings.bgColor);

    formatting();

    if(settings.toggleTextFlagMask) {
        layer_set_hidden(bat_indicator, true);
        layer_set_hidden(date_layer, true);
        layer_set_hidden(time_layer, true);
    } else {
        layer_set_hidden(bat_indicator, !settings.showBatBar);
        layer_set_hidden(date_layer, !settings.doDate);
        layer_set_hidden(time_layer, false);
    }

    layer_mark_dirty(bat_indicator);
    layer_mark_dirty(date_layer);
    layer_mark_dirty(time_layer);

    update_flag();
    update_time();
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