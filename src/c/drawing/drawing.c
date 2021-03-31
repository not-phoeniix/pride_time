#include <pebble.h>
#include "drawing.h"
#include "../main.h"

extern Window *main_window;

extern ClaySettings settings;

extern int *flag_colors[];
extern int num_stripes[];

extern int battery_level;
extern int date_bool_offset;
extern int no_bat_offset;
extern float flag_stripe_scale;
extern int flag_stripe_offset;

char time_char[] = "00:00";
char date_char[] = "MM/DD";

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
    graphics_context_set_fill_color(ctx, GColorBlack);
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

static void draw_bat_bar(GContext *ctx) {
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

void flag_update_proc(Layer *layer, GContext *ctx) {
    draw_flag(num_stripes[settings.flag_number], flag_colors[settings.flag_number], flag_stripe_scale, flag_stripe_offset, ctx);
}

void time_draw_update_proc(Layer *layer, GContext *ctx) {
    draw_time(ctx);
}

void date_draw_update_proc(Layer *layer, GContext *ctx) {
    draw_date(ctx);
}

void battery_update_proc(Layer *layer, GContext *ctx) {
    draw_bat_bar(ctx);
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
}