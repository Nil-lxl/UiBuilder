/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"
#include "lv_aic_player.h"
#include <math.h>

// Global variables
static screen_gif_t *scr = NULL;
static lv_obj_t *current_player = NULL;  // Currently used player

// Handle gesture events - swipe right to return to Scene_screen
static void screen_gif_on_gesture(lv_event_t *e)
{
    lv_obj_t *act_scr = lv_scr_act();
    if (screen_is_loading(act_scr)) return;

    // Clean current screen's children
    lv_obj_clean(act_scr);
    // Create Scene_screen
    Scene_screen_create(&ui_manager);
    // Load Scene_screen with no animation
    lv_scr_load_anim(Scene_screen_get(&ui_manager)->obj,
                     LV_SCR_LOAD_ANIM_NONE, 0, 0, ui_manager.auto_del);
}

// Initialize when loading the custom screen
void screen_gif_custom_load_start(void) {
    scr = screen_gif_get(&ui_manager);
    if (!scr || !scr->obj) {
        return;
    }

    // Get selected image from previous screen to determine which player to show
    int selected_image = scene_get_selected_apng_image();
    
    // Create appropriate player based on selection
    if (selected_image == 1) {
        current_player = lv_aic_player_create(scr->obj);
        lv_aic_player_set_src(current_player, LVGL_IMAGE_PATH(test.png));
        lv_aic_player_set_auto_restart(current_player, true);
        lv_aic_player_set_cmd(current_player, LV_AIC_PLAYER_CMD_START, NULL);
        lv_obj_set_pos(current_player, 0, 0);
        lv_obj_set_size(current_player, 480, 480);
    } else if (selected_image == 2) {
        // Create player for sd.png
        current_player = lv_aic_player_create(scr->obj);
        lv_aic_player_set_src(current_player, LVGL_IMAGE_PATH(sd.png));
        lv_aic_player_set_auto_restart(current_player, true);
        lv_aic_player_set_cmd(current_player, LV_AIC_PLAYER_CMD_START, NULL);
        lv_obj_set_pos(current_player, 0, 0);
        lv_obj_set_size(current_player, 480, 480);
    }

    // Add gesture event callback for right swipe navigation
    lv_obj_add_event_cb(scr->obj, screen_gif_on_gesture, LV_EVENT_GESTURE, NULL);
}

// Clean up when screen is unloaded
void screen_gif_custom_unloaded(void) {
    // Reset player pointer on unload
    current_player = NULL;
}
