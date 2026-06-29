/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"

static main_screen_t *scr = NULL;

// Animation parameters
#define ZOOM_START        150
#define ZOOM_END          256
#define SCALE_TIME_MS     300
#define FADE_TIME_MS      250
#define STAGGER_MS        60
#define FADE_LEAD_MS      30

// Scale animation callback
static void scale_anim_cb(void *var, int32_t v)
{
    lv_img_set_zoom(var, (uint32_t)v);
}

// Opacity animation callback
static void opa_anim_cb(void *var, int32_t v)
{
    lv_obj_set_style_opa(var, (lv_opa_t)v, 0);
}

// Create entrance animation: scale + fade-in effect
static void create_entrance_anim(lv_obj_t *container, lv_obj_t *img, uint32_t delay)
{
    lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);

    lv_img_set_zoom(img, ZOOM_START);
    lv_obj_set_style_opa(img, LV_OPA_TRANSP, 0);

    // Scale animation
    lv_anim_t a_scale;
    lv_anim_init(&a_scale);
    lv_anim_set_var(&a_scale, img);
    lv_anim_set_time(&a_scale, SCALE_TIME_MS);
    lv_anim_set_delay(&a_scale, delay);
    lv_anim_set_values(&a_scale, ZOOM_START, ZOOM_END);
    lv_anim_set_exec_cb(&a_scale, scale_anim_cb);
    lv_anim_set_path_cb(&a_scale, lv_anim_path_ease_out);
    lv_anim_set_early_apply(&a_scale, true);
    lv_anim_start(&a_scale);

    // Fade-in animation
    lv_anim_t a_opa;
    lv_anim_init(&a_opa);
    lv_anim_set_var(&a_opa, img);
    lv_anim_set_time(&a_opa, FADE_TIME_MS);
    lv_anim_set_delay(&a_opa, delay + FADE_LEAD_MS);
    lv_anim_set_values(&a_opa, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a_opa, opa_anim_cb);
    lv_anim_set_path_cb(&a_opa, lv_anim_path_ease_out);
    lv_anim_set_early_apply(&a_opa, true);
    lv_anim_start(&a_opa);

    lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
}

// Hide all containers
static void hide_all_containers(void)
{
    if (!scr) return;

    lv_obj_add_flag(scr->container_scene, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(scr->container_light, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(scr->container_air,   LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(scr->container_blind, LV_OBJ_FLAG_HIDDEN);
}

// Gesture event handling: swipe right to return to startup screen
static void main_screen_on_gesture(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_obj_t *act_scr = lv_scr_act();

    if (screen_is_loading(act_scr)) {
        return;
    }

    if (dir == LV_DIR_RIGHT) {
        // Swipe right to return to startup screen
        startup_screen_create(&ui_manager);
        startup_screen_t *startup_scr = startup_screen_get(&ui_manager);
        lv_obj_clear_flag(startup_scr->obj, LV_OBJ_FLAG_HIDDEN);
        lv_scr_load_anim(startup_scr->obj,
                         LV_SCR_LOAD_ANIM_MOVE_RIGHT, 350, 0, ui_manager.auto_del);
    }
}

// Enable gesture bubble for child objects
static void enable_gesture_bubble_for_children(void)
{
    lv_obj_add_event_cb(scr->obj, main_screen_on_gesture, LV_EVENT_GESTURE, NULL);

    // Scene container
    lv_obj_add_flag(scr->container_scene, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_scene,    LV_OBJ_FLAG_GESTURE_BUBBLE);

    // Light container
    lv_obj_add_flag(scr->container_light, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_light,     LV_OBJ_FLAG_GESTURE_BUBBLE);

    // Air conditioning container
    lv_obj_add_flag(scr->container_air, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_air,     LV_OBJ_FLAG_GESTURE_BUBBLE);

    // Curtain container
    lv_obj_add_flag(scr->container_blind, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_blind,     LV_OBJ_FLAG_GESTURE_BUBBLE);

    // Disable scrolling
    lv_obj_clear_flag(scr->container_scene, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(scr->container_light, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(scr->container_air,   LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(scr->container_blind, LV_OBJ_FLAG_SCROLLABLE);
}

// Custom initialization when main screen loads
void main_screen_custom_load_start(void)
{
    scr = main_screen_get(&ui_manager);
    if (!scr) return;

    hide_all_containers();

    // Play entrance animations for four containers in sequence
    create_entrance_anim(scr->container_scene, scr->image_scene, 0 * STAGGER_MS);
    create_entrance_anim(scr->container_light, scr->image_light, 1 * STAGGER_MS);
    create_entrance_anim(scr->container_air,   scr->image_air,   2 * STAGGER_MS);
    create_entrance_anim(scr->container_blind, scr->image_blind, 3 * STAGGER_MS);

    enable_gesture_bubble_for_children();
}
