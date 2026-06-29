/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"

// Global variables
static air_screen_t *scr = NULL;
static bool is_air_on = 0; // 0 means off, 1 means on

// Gesture callback: Swipe right to return to main screen
static void air_screen_gesture_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_obj_t *target = lv_event_get_target(e);

    if (lv_obj_has_class(target, &lv_slider_class)) {
        return;
    }

    if (dir == LV_DIR_RIGHT) {
        ui_manager_t *ui = &ui_manager;
        main_screen_create(ui);
        lv_scr_load_anim(main_screen_get(ui)->obj,
                         LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, ui->auto_del);
    }
}

// Air conditioner animation frame image path array
static const char *a[17] = {
    LVGL_IMAGE_PATH(air/1.png),
    LVGL_IMAGE_PATH(air/2.png),
    LVGL_IMAGE_PATH(air/3.png),
    LVGL_IMAGE_PATH(air/4.png),
    LVGL_IMAGE_PATH(air/5.png),
    LVGL_IMAGE_PATH(air/6.png),
    LVGL_IMAGE_PATH(air/7.png),
    LVGL_IMAGE_PATH(air/8.png),
    LVGL_IMAGE_PATH(air/9.png),
    LVGL_IMAGE_PATH(air/10.png),
    LVGL_IMAGE_PATH(air/11.png),
    LVGL_IMAGE_PATH(air/12.png),
    LVGL_IMAGE_PATH(air/13.png),
    LVGL_IMAGE_PATH(air/14.png),
    LVGL_IMAGE_PATH(air/15.png),
    LVGL_IMAGE_PATH(air/16.png),
    LVGL_IMAGE_PATH(air/17.png),
};

static lv_timer_t *timer = NULL;
static int value = 0;
static int dir = 1;

// Air conditioner animation timer callback
void air_screen_timer_cb(lv_timer_t *timer)
{
    if (!scr) return;

    value += dir;
    if (value >= 16) {
        value = 16;
        dir = -1;
    } else if (value <= 0) {
        value = 0;
        dir = 1;
    }

    lv_img_set_src(scr->image_ac, a[value]);
}

// Cleanup when interface is unloaded
void air_screen_custom_unloaded(void)
{
    if (timer != NULL) {
        lv_timer_del(timer);
        timer = NULL;
    }
    is_air_on = false;
}

// Initialization when interface is loaded
void air_screen_custom_load_start(void)
{
    scr = air_screen_get(&ui_manager);

    lv_obj_add_event_cb(scr->obj, air_screen_gesture_cb, LV_EVENT_GESTURE, NULL);

    lv_obj_add_flag(scr->image_cold, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(scr->image_wind, LV_OBJ_FLAG_HIDDEN);

    lv_img_set_src(scr->image_ac, LVGL_IMAGE_PATH(air/0.png));
}

// Temperature down button click
void air_screen_button_down_custom_clicked(void)
{
    uint16_t selected = lv_roller_get_selected(scr->roller_temp);
    uint16_t option_count = lv_roller_get_option_cnt(scr->roller_temp);
    uint16_t next_index = (selected + 1) % option_count;

    lv_roller_set_selected(scr->roller_temp, next_index, LV_ANIM_ON);
}

// Temperature up button click
void air_screen_button_up_custom_clicked(void)
{
    uint16_t selected = lv_roller_get_selected(scr->roller_temp);
    uint16_t option_count = lv_roller_get_option_cnt(scr->roller_temp);
    uint16_t prev_index = (selected == 0) ? option_count - 1 : selected - 1;

    lv_roller_set_selected(scr->roller_temp, prev_index, LV_ANIM_ON);
}

// Cooling mode button click
void air_screen_button_cold_custom_clicked(void)
{
    if (is_air_on) {
        lv_img_set_src(scr->image_cold, LVGL_IMAGE_PATH(air_icon/cold11.png));
        lv_obj_clear_flag(scr->image_cold, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(scr->image_wind, LV_OBJ_FLAG_HIDDEN);
    }
}

// Wind mode button click
void air_screen_button_wind_custom_clicked(void)
{
    if (is_air_on) {
        lv_img_set_src(scr->image_wind, LVGL_IMAGE_PATH(air_icon/wind1.png));
        lv_obj_clear_flag(scr->image_wind, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(scr->image_cold, LV_OBJ_FLAG_HIDDEN);
    }
}

// Power switch button click
void air_screen_button_power_custom_clicked(void)
{
    is_air_on = !is_air_on;

    if (is_air_on) {
        // Turn on air conditioner
        if (timer == NULL) {
            timer = lv_timer_create(air_screen_timer_cb, 100, NULL);
        }
        lv_obj_clear_flag(scr->image_cold, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_img_recolor_opa(scr->image_ac, LV_OPA_0, LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        // Turn off air conditioner
        if (timer != NULL) {
            lv_timer_del(timer);
            timer = NULL;
        }
        lv_obj_add_flag(scr->image_cold, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(scr->image_wind, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(scr->image_ac, LVGL_IMAGE_PATH(air/0.png));
    }
}
