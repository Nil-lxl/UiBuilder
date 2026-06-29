/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"

// Variable to record which image was clicked, used by apng_custom.c
// 0: not set, 1: image_screen1 (test.png), 2: image_screen2 (sd.png)
static int selected_apng_image = 0;

static Scene_screen_t *scr = NULL;

// Get selected image ID (called by apng_custom.c)
int scene_get_selected_apng_image(void)
{
    return selected_apng_image;
}

// Scene image 1 click
void Scene_screen_image_screen1_custom_clicked(void)
{
    selected_apng_image = 1;
}

// Scene image 2 click
void Scene_screen_image_screen2_custom_clicked(void)
{
    selected_apng_image = 2;
}

// Scene image 3 click: toggle selection state
void Scene_screen_image_screen3_custom_clicked(void)
{
    if (!scr) return;
    static bool flag = false;

    if (!flag) {
        lv_img_set_src(scr->image_screen3, LVGL_IMAGE_PATH(a/3_b.png));
        flag = true;
    } else {
        lv_img_set_src(scr->image_screen3, LVGL_IMAGE_PATH(a/3.png));
        flag = false;
    }
}

// Scene image 4 click: toggle selection state
void Scene_screen_image_screen4_custom_clicked(void)
{
    if (!scr) return;
    static bool flag = false;

    if (!flag) {
        lv_img_set_src(scr->image_screen4, LVGL_IMAGE_PATH(a/4_b.png));
        flag = true;
    } else {
        lv_img_set_src(scr->image_screen4, LVGL_IMAGE_PATH(a/4.png));
        flag = false;
    }
}

// Scene image 5 click: toggle selection state
void Scene_screen_image_screen5_custom_clicked(void)
{
    if (!scr) return;
    static bool flag = false;

    if (!flag) {
        lv_img_set_src(scr->image_screen5, LVGL_IMAGE_PATH(a/5_b.png));
        flag = true;
    } else {
        lv_img_set_src(scr->image_screen5, LVGL_IMAGE_PATH(a/5.png));
        flag = false;
    }
}

// Gesture event callback: swipe right to return to main screen
static void scene_on_gesture(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

    if (dir != LV_DIR_RIGHT) {
        return;
    }

    lv_obj_t *act_scr = lv_scr_act();
    if (screen_is_loading(act_scr)) {
        return;
    }

    main_screen_create(&ui_manager);
    lv_scr_load_anim(main_screen_get(&ui_manager)->obj,
                     LV_SCR_LOAD_ANIM_MOVE_RIGHT, 350, 0, ui_manager.auto_del);
}

// Initialize when interface loads: attach gesture and enable bubbling
void Scene_screen_custom_load_start(void)
{
    scr = Scene_screen_get(&ui_manager);

    // Root object listens for gestures
    lv_obj_add_event_cb(scr->obj, scene_on_gesture, LV_EVENT_GESTURE, NULL);

    // Container enables gesture bubble
    lv_obj_add_flag(scr->container_board, LV_OBJ_FLAG_GESTURE_BUBBLE);

    // Images enable gesture bubble
    lv_obj_add_flag(scr->image_screen1, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_screen2, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_screen3, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_screen4, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->image_screen5, LV_OBJ_FLAG_GESTURE_BUBBLE);

    // Labels enable gesture bubble
    lv_obj_add_flag(scr->label_temp2,    LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_IT,       LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_temp1,    LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_room,     LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_percent,  LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_indoor,   LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_screen1,  LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_screen2,  LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_screen3,  LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_screen4,  LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(scr->label_screen5,  LV_OBJ_FLAG_GESTURE_BUBBLE);

}
