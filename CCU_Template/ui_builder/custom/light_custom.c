/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"

#define DURATION_SHORT   280
#define DURATION_MEDIUM  380
#define STAGGER          100

// Color item structure: contains object pointer, small image and large image paths
typedef struct {
    lv_obj_t **obj;
    const char *small;
    const char *big;
} ColorItem;

static ColorItem s_colors[5];
static int current_selection = -1;
static light_screen_t *scr = NULL;

// Light color image path array
static const char *a[5] = {
    LVGL_IMAGE_PATH(blue_light.png),
    LVGL_IMAGE_PATH(green_light.png),
    LVGL_IMAGE_PATH(pure_light.png),
    LVGL_IMAGE_PATH(red_light.png),
    LVGL_IMAGE_PATH(yellow_light.png),
};

// Map brightness percentage to opacity value (range 40-220)
static inline lv_opa_t map_brightness_opa(int val_percent)
{
    if (val_percent < 0)   val_percent = 0;
    if (val_percent > 100) val_percent = 100;

    int32_t opa = 40 + (val_percent * (220 - 40)) / 100;
    return (lv_opa_t)opa;
}

// Brightness slider value change callback
void light_screen_slider_brightness_custom_value_changed(void)
{
    if (!scr) return;

    int value = lv_slider_get_value(scr->slider_brightness);
    lv_obj_set_style_img_opa(scr->image_light, map_brightness_opa(value), LV_STATE_DEFAULT);
}

// Apply color selection
static inline void light_apply_selection(int idx)
{
    if (!scr) return;

    // Clicking the same color again turns off the light
    if (current_selection == idx) {
        lv_img_set_src(scr->image_light, LVGL_IMAGE_PATH(UI/light/light.png));
        lv_slider_set_value(scr->slider_brightness, 0, LV_ANIM_OFF);
        light_screen_slider_brightness_custom_value_changed();
        idx = -1;
    }

    for (int i = 0; i < 5; ++i) {
        lv_img_set_src(*s_colors[i].obj, (i == idx) ? s_colors[i].big : s_colors[i].small);
        if (i == idx) {
            lv_img_set_src(scr->image_light, a[i]);
            lv_slider_set_value(scr->slider_brightness, 100, LV_ANIM_OFF);
            light_screen_slider_brightness_custom_value_changed();
        }
    }

    current_selection = idx;
}

// Common animation settings
static inline void anim_apply_common(lv_anim_t *a, void *var, uint32_t time, uint32_t delay,
                                     lv_anim_exec_xcb_t exec, int32_t from, int32_t to)
{
    lv_anim_init(a);
    lv_anim_set_var(a, var);
    lv_anim_set_time(a, time);
    lv_anim_set_delay(a, delay);
    lv_anim_set_values(a, from, to);
    lv_anim_set_exec_cb(a, exec);
    lv_anim_set_path_cb(a, lv_anim_path_ease_out);
    lv_anim_start(a);
}

// Animation execution callbacks
static void exec_y(void *var, int32_t v)   { lv_obj_set_y(var, v); }
static void exec_opa(void *var, int32_t v) { lv_obj_set_style_opa(var, (lv_opa_t)v, 0); }

// Y-axis move-in + fade-in animation
static void animate_move_in_y_with_fade(lv_obj_t *obj, int16_t delta, uint16_t delay)
{
    lv_coord_t y0 = lv_obj_get_y(obj);
    lv_obj_set_y(obj, y0 + delta);
    lv_obj_set_style_opa(obj, LV_OPA_0, 0);

    lv_anim_t a_move, a_fade;
    anim_apply_common(&a_move, obj, DURATION_MEDIUM, delay, exec_y, y0 + delta, y0);
    anim_apply_common(&a_fade, obj, DURATION_SHORT,  delay + 60, exec_opa, LV_OPA_0, LV_OPA_COVER);
}

// Gesture event callback: swipe right to return to main screen
static void light_screen_gesture_cb(lv_event_t *e)
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

// Initialize when interface loads
void light_screen_custom_load_start(void)
{
    scr = light_screen_get(&ui_manager);
    if (!scr) return;

    const int16_t delta_bottom = +24;
    const int16_t delta_light  = -16;

    // Play entrance animation
    animate_move_in_y_with_fade(scr->image_clor, delta_bottom, 0 * STAGGER);
    animate_move_in_y_with_fade(scr->image_light, delta_light,  1 * STAGGER);

    // Initialize color item configuration - refactor to loop
    const ColorItem color_configs[] = {
        {&scr->image_blue,   LVGL_IMAGE_PATH(light_change/blue.png), LVGL_IMAGE_PATH(blue_b.png)},
        {&scr->image_greeen, LVGL_IMAGE_PATH(light_change/green.png), LVGL_IMAGE_PATH(green_b.png)},
        {&scr->image_pure,   LVGL_IMAGE_PATH(light_change/pure.png),  LVGL_IMAGE_PATH(pure_b.png)},
        {&scr->image_red,    LVGL_IMAGE_PATH(light_change/red.png),   LVGL_IMAGE_PATH(red_b.png)},
        {&scr->image_yellow, LVGL_IMAGE_PATH(light_change/yellow.png), LVGL_IMAGE_PATH(yellow_b.png)}
    };

    for (int i = 0; i < 5; i++) {
        s_colors[i] = color_configs[i];
    }

    lv_obj_add_event_cb(scr->obj, light_screen_gesture_cb, LV_EVENT_GESTURE, NULL);

    int value = lv_slider_get_value(scr->slider_brightness);
    lv_obj_set_style_img_opa(scr->image_light, map_brightness_opa(value), LV_STATE_DEFAULT);
}

// Blue button click
void light_screen_image_blue_custom_clicked(void)
{
    light_apply_selection(0);
}

// Green button click
void light_screen_image_green_custom_clicked(void)
{
    light_apply_selection(1);
}

// White button click
void light_screen_image_pure_custom_clicked(void)
{
    light_apply_selection(2);
}

// Red button click
void light_screen_image_red_custom_clicked(void)
{
    light_apply_selection(3);
}

// Yellow button click
void light_screen_image_yellow_custom_clicked(void)
{
    light_apply_selection(4);
}
