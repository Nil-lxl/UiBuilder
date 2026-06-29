/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

// 1.0.1 add animal
#include "custom.h"
#include "lv_aic_player.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Static variable definitions
static startup_screen_t *scr = NULL;
static lv_timer_t *clock_timer = NULL;
static int current_hour = 10;
static int current_minute = 15;

// Update clock display
static void update_clock_display(void)
{
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", current_hour, current_minute);
    lv_label_set_text(scr->label_time, time_str);
}

// Clock timer callback, update time every minute
static void clock_timer_cb(lv_timer_t *timer)
{
    current_minute++;

    if (current_minute >= 60) {
        current_minute = 0;
        current_hour++;

        if (current_hour >= 24) {
            current_hour = 0;
        }
    }

    update_clock_display();
}

// Custom initialization when startup screen loads
void startup_screen_custom_load_start(void)
{
    scr = startup_screen_get(&ui_manager);
    if (!scr || !scr->label_time) {
        return;
    }

    // Parse current time from label text
    const char *current_text = lv_label_get_text(scr->label_time);
    if (current_text && strlen(current_text) >= 5) {
        sscanf(current_text, "%d:%d", &current_hour, &current_minute);
    } else {
        current_hour = 10;
        current_minute = 15;
    }

    // Create or reset clock timer
    if (clock_timer) {
        lv_timer_reset(clock_timer);
    } else {
        clock_timer = lv_timer_create(clock_timer_cb, 5000, NULL);
    }
    update_clock_display();
}

// Custom cleanup when startup screen unloads
void startup_screen_custom_unloaded(void)
{
    if (scr) {
        scr->player_pinwheel = NULL;
        scr->player_2 = NULL;
    }
    // Clean up clock timer
    if (clock_timer) {
        lv_timer_delete(clock_timer);
        clock_timer = NULL;
    }
}
