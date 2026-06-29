/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"

// Curtain animation frame image path array
const char *a[] = {
    LVGL_IMAGE_PATH(1.png),
    LVGL_IMAGE_PATH(3.png),
    LVGL_IMAGE_PATH(5.png),
    LVGL_IMAGE_PATH(7.png),
    LVGL_IMAGE_PATH(9.png),
    LVGL_IMAGE_PATH(11.png),
    LVGL_IMAGE_PATH(13.png),
    LVGL_IMAGE_PATH(15.png),
    LVGL_IMAGE_PATH(17.png),
    LVGL_IMAGE_PATH(19.png),
    LVGL_IMAGE_PATH(21.png),
    LVGL_IMAGE_PATH(23.png),
    LVGL_IMAGE_PATH(25.png),
    LVGL_IMAGE_PATH(27.png),
    LVGL_IMAGE_PATH(29.png),
    LVGL_IMAGE_PATH(31.png),
    LVGL_IMAGE_PATH(33.png),
    LVGL_IMAGE_PATH(35.png),
    LVGL_IMAGE_PATH(37.png),
    LVGL_IMAGE_PATH(39.png),
    LVGL_IMAGE_PATH(41.png),
    LVGL_IMAGE_PATH(43.png),
    LVGL_IMAGE_PATH(45.png),
    LVGL_IMAGE_PATH(47.png),
    LVGL_IMAGE_PATH(49.png),
    LVGL_IMAGE_PATH(51.png),
    LVGL_IMAGE_PATH(53.png),
    LVGL_IMAGE_PATH(55.png),
    LVGL_IMAGE_PATH(57.png),
    LVGL_IMAGE_PATH(59.png),
    LVGL_IMAGE_PATH(61.png),
    LVGL_IMAGE_PATH(63.png),
    LVGL_IMAGE_PATH(65.png),
    LVGL_IMAGE_PATH(67.png),
    LVGL_IMAGE_PATH(68.png),
    LVGL_IMAGE_PATH(69.png),
    LVGL_IMAGE_PATH(70.png),
    LVGL_IMAGE_PATH(71.png),
    LVGL_IMAGE_PATH(72.png),
    LVGL_IMAGE_PATH(73.png),
    LVGL_IMAGE_PATH(74.png),
    LVGL_IMAGE_PATH(75.png),
};

// Calculate number of array elements
#define A_NUM (sizeof(a) / sizeof(a[0]))

static blind_screen_t *scr = NULL;

static lv_timer_t *blind_open_timer = NULL;
static lv_timer_t *blind_close_timer = NULL;

// Manually open one frame
void blind_screen_slider_open(void)
{
    if (!scr) return;
    int32_t value = lv_slider_get_value(scr->slider_power);

    if (value < (A_NUM - 1)) {
        value++;
        lv_img_set_src(scr->image_blind, a[value]);
        lv_slider_set_value(scr->slider_power, value, LV_ANIM_ON);
    }
}

// Manually close one frame
void blind_screen_slider_close(void)
{
    if (!scr) return;
    int32_t value = lv_slider_get_value(scr->slider_power);

    if (value > 0) {
        value--;
        lv_img_set_src(scr->image_blind, a[value]);
        lv_slider_set_value(scr->slider_power, value, LV_ANIM_ON);
    }
}

// Auto open timer callback
void blind_screen_timer_open_cb(lv_timer_t *timer)
{
    if (!scr || !scr->obj) {
        lv_timer_del(timer);
        blind_open_timer = NULL;
        return;
    }

    int32_t value = lv_slider_get_value(scr->slider_power);

    if (value < (A_NUM - 1)) {
        value++;
        lv_img_set_src(scr->image_blind, a[value]);
        lv_slider_set_value(scr->slider_power, value, LV_ANIM_OFF);
    } else {
        lv_timer_del(timer);
        blind_open_timer = NULL;
    }
}

// Auto close timer callback
void blind_screen_timer_close_cb(lv_timer_t *timer)
{
    if (!scr || !scr->obj) {
        lv_timer_del(timer);
        blind_close_timer = NULL;
        return;
    }

    int32_t value = lv_slider_get_value(scr->slider_power);

    if (value > 0) {
        value--;
        lv_img_set_src(scr->image_blind, a[value]);
        lv_slider_set_value(scr->slider_power, value, LV_ANIM_OFF);
    } else {
        lv_timer_del(timer);
        blind_close_timer = NULL;
    }
}

// Open button click: start auto open animation
void blind_screen_button_open_custom_clicked(void)
{
    if (!scr) return;

    if (blind_open_timer) {
        lv_timer_del(blind_open_timer);
        blind_open_timer = NULL;
    }

    // Stop reverse animation to avoid conflicts
    if (blind_close_timer) {
        lv_timer_del(blind_close_timer);
        blind_close_timer = NULL;
    }

    blind_open_timer = lv_timer_create(blind_screen_timer_open_cb, 30, NULL);
}

// Close button click: start auto close animation
void blind_screen_button_close_custom_clicked(void)
{
    if (!scr) return;

    if (blind_close_timer) {
        lv_timer_del(blind_close_timer);
        blind_close_timer = NULL;
    }

    // Stop reverse animation to avoid conflicts
    if (blind_open_timer) {
        lv_timer_del(blind_open_timer);
        blind_open_timer = NULL;
    }

    blind_close_timer = lv_timer_create(blind_screen_timer_close_cb, 30, NULL);
}

// Slider value change callback: synchronize curtain image update
void blind_screen_slider_power_custom_value_changed(void)
{
    if (!scr) return;

    int32_t value = lv_slider_get_value(scr->slider_power);

    lv_img_set_src(scr->image_blind, a[value]);
}

// Gesture event callback: swipe right to return to main screen
static void blind_screen_gesture_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_obj_t *orig_target = lv_event_get_target(e);

    // Ignore if gesture originates from slider or its child objects
    lv_obj_t *p = orig_target;
    while (p) {
        if (lv_obj_has_class(p, &lv_slider_class)) {
            return;
        }
        p = lv_obj_get_parent(p);
    }

    if (dir == LV_DIR_RIGHT) {
        ui_manager_t *ui = &ui_manager;
        main_screen_create(ui);
        lv_scr_load_anim(main_screen_get(ui)->obj,
                         LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, ui->auto_del);
    }
}

// Prevent slider gesture bubbling
static void blind_slider_stop_gesture_bubbling(lv_event_t *e)
{
    lv_event_stop_bubbling(e);
}

// Stop all timers
static void blind_stop_all_timers(void)
{
    if (blind_open_timer) {
        lv_timer_del(blind_open_timer);
        blind_open_timer = NULL;
    }

    if (blind_close_timer) {
        lv_timer_del(blind_close_timer);
        blind_close_timer = NULL;
    }
}

// Slider user interaction callback: stop auto animation
static void blind_slider_user_interact_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING) {
        blind_stop_all_timers();
    }
}

// Initialize when interface loads
void blind_screen_custom_load_start(void)
{
    scr = blind_screen_get(&ui_manager);
    if (!scr) return;

    lv_obj_add_event_cb(scr->obj, blind_screen_gesture_cb, LV_EVENT_GESTURE, NULL);

    // Configure slider
    if (scr->slider_power) {
        lv_slider_set_range(scr->slider_power, 0, A_NUM - 1);
        lv_obj_remove_flag(scr->slider_power, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_event_cb(scr->slider_power, blind_slider_stop_gesture_bubbling, LV_EVENT_GESTURE, NULL);
        lv_obj_add_event_cb(scr->slider_power, blind_slider_user_interact_cb, LV_EVENT_PRESSED, NULL);
        lv_obj_add_event_cb(scr->slider_power, blind_slider_user_interact_cb, LV_EVENT_PRESSING, NULL);
    }

    blind_open_timer = NULL;
    blind_close_timer = NULL;
}

// Clean up when interface unloads
void blind_screen_custom_unloaded(void)
{
    if (blind_open_timer) {
        lv_timer_del(blind_open_timer);
        blind_open_timer = NULL;
    }

    if (blind_close_timer) {
        lv_timer_del(blind_close_timer);
        blind_close_timer = NULL;
    }

    if (scr && scr->obj) {
        lv_obj_remove_event_cb(scr->obj, blind_screen_gesture_cb);
    }

    scr = NULL;
}
