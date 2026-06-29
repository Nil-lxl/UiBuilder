/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"
#include "obj_drag/lv_obj_drag.h"

/* ========== Macro Definitions ========== */

#define CAR_IMG_WIDTH       411
#define CAR_IMG_HEIGHT      242
#define CAR_DISTRIBUTION_H  826
#define SYNC_PERIOD_MS      16
#define POSITION_TOLERANCE  3

/* ========== Static Data ========== */

// Mode text
static const char *mode_text[] = {
    "街道模式",
    "运动模式", 
    "竞速模式"
};

// Car images
static const char *car_image[] = {
    LVGL_IMAGE_PATH(screen1/car1.png),
    LVGL_IMAGE_PATH(screen1/car2.png),
    LVGL_IMAGE_PATH(screen1/car3.png)
};

/* ========== Static Variables ========== */

// Roller state
static uint8_t index_img = 0;
static uint8_t last_selected_idx = 1;
static uint8_t is_created = 0;

// UI objects
static lv_obj_t *text_roller = NULL;
static lv_obj_t *car_img[3] = {NULL, NULL, NULL};

// Sync timer
static lv_timer_t *sync_timer = NULL;

// Hint animation
static lv_obj_t *hint_trigger_objs[6] = {NULL};  // Hint animation trigger objects

/* ========== Function Forward Declarations ========== */

// Hint animation
static void show_developing_hint_screen1(void);
static void hint_pressed_cb_screen1(lv_event_t *e);
static void bind_hint_event_screen1(lv_obj_t *obj);
static void hint_animation_init_screen1(screen_1_t *scr);

// Car roller
static void sync_timer_cb(lv_timer_t *timer);
static void sync_car_to_text(void);
static void text_roller_change_cb(lv_event_t *e);
static void text_roller_pressing_cb(lv_event_t *e);
static void text_roller_released_cb(lv_event_t *e);

// Screen switch
static void car_click_cb(lv_event_t *e);
static void switch_to_screen(uint8_t screen_idx);

// Cleanup
static void screen_1_roller_cleanup(void);

/* ========== Hint Animation Management ========== */

static void show_developing_hint_screen1(void)
{
    screen_1_t *scr = screen_1_get(&ui_manager);
    if (!scr || !scr->label_wait) {
        return;
    }
    
    lv_obj_t *label = scr->label_wait;
    if (!lv_obj_is_valid(label)) {
        return;
    }
    
    lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_obj_set_y);
    
    lv_obj_set_pos(label, 384, 18);
    
    lv_anim_t anim_x;
    lv_anim_init(&anim_x);
    lv_anim_set_var(&anim_x, label);
    lv_anim_set_exec_cb(&anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&anim_x, 384, 384);
    lv_anim_set_time(&anim_x, 1500);
    lv_anim_set_repeat_count(&anim_x, 1);
    lv_anim_set_delay(&anim_x, 1000);
    lv_anim_set_path_cb(&anim_x, lv_anim_path_linear);
    lv_anim_start(&anim_x);

    lv_anim_t anim_y;
    lv_anim_init(&anim_y);
    lv_anim_set_var(&anim_y, label);
    lv_anim_set_exec_cb(&anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&anim_y, 18, -30);
    lv_anim_set_time(&anim_y, 1500);
    lv_anim_set_repeat_count(&anim_y, 1);
    lv_anim_set_delay(&anim_y, 1000);
    lv_anim_set_path_cb(&anim_y, lv_anim_path_linear);
    lv_anim_start(&anim_y);
}

static void hint_pressed_cb_screen1(lv_event_t *e)
{
    (void)e;
    show_developing_hint_screen1();
}

static void bind_hint_event_screen1(lv_obj_t *obj)
{
    if (!obj || !lv_obj_is_valid(obj)) {
        return;
    }
    lv_obj_add_event_cb(obj, hint_pressed_cb_screen1, LV_EVENT_PRESSED, NULL);
}

static void hint_animation_init_screen1(screen_1_t *scr)
{
    hint_trigger_objs[0] = scr->container_camera;
    hint_trigger_objs[1] = scr->image_camera;
    hint_trigger_objs[2] = scr->container_phone;
    hint_trigger_objs[3] = scr->image_phone;
    hint_trigger_objs[4] = scr->container_play;
    hint_trigger_objs[5] = scr->image_play;
    
    for (int i = 0; i < 6; i++) {
        bind_hint_event_screen1(hint_trigger_objs[i]);
    }
}

/* ========== Car Roller ========== */

static void sync_car_to_text(void)
{
    if (text_roller == NULL || !lv_obj_is_valid(text_roller)) {
        return;
    }
    
    for (int i = 0; i < 3; i++) {
        if (car_img[i] == NULL || !lv_obj_is_valid(car_img[i])) {
            return;
        }
    }
    
    lv_obj_drag_t *text_drag = (lv_obj_drag_t *)text_roller;
    int32_t text_container_h = lv_obj_get_height(text_roller);
    if (text_container_h <= 0) {
        return;
    }
    
    lv_area_t text_area;
    lv_obj_get_coords(text_roller, &text_area);
    int32_t text_center_abs = (text_area.y1 + text_area.y2) / 2;
    
    lv_obj_t *car_container = lv_obj_get_parent(car_img[0]);
    if (!car_container || !lv_obj_is_valid(car_container)) {
        return;
    }
    
    lv_area_t car_cont_area;
    lv_obj_get_coords(car_container, &car_cont_area);
    int32_t car_cont_center_abs = (car_cont_area.y1 + car_cont_area.y2) / 2;
    
    uint32_t child_cnt = lv_obj_get_child_cnt(text_roller);
    
    for (uint32_t i = 0; i < child_cnt && i < 3; i++) {
        if (car_img[i] == NULL || !lv_obj_is_valid(car_img[i])) {
            continue;
        }
        
        lv_obj_t *text_child = lv_obj_get_child(text_roller, i);
        if (!text_child || !lv_obj_is_valid(text_child)) {
            continue;
        }
        
        lv_area_t text_child_area;
        lv_obj_get_coords(text_child, &text_child_area);
        int32_t text_child_center_abs = (text_child_area.y1 + text_child_area.y2) / 2;
        int32_t text_child_rel_y = text_child_center_abs - text_area.y1;
        int32_t car_offset_from_center = 
            (text_child_rel_y * CAR_DISTRIBUTION_H / text_container_h) - (CAR_DISTRIBUTION_H / 2);
        
        if (LV_ABS(car_offset_from_center) <= POSITION_TOLERANCE) {
            car_offset_from_center = 0;
        }
       
        int32_t target_car_center_abs = car_cont_center_abs + car_offset_from_center;
        int32_t car_y = target_car_center_abs - CAR_IMG_HEIGHT / 2 - car_cont_area.y1;
        lv_obj_set_y(car_img[i], car_y);
        
        int32_t text_offset_from_center = text_child_center_abs - text_center_abs;
        if (LV_ABS(text_offset_from_center) <= POSITION_TOLERANCE) {
            text_offset_from_center = 0;
        }
        
        int32_t offset_abs = LV_ABS(text_offset_from_center);
        if (offset_abs > text_drag->zoom_change_boundary) {
            offset_abs = text_drag->zoom_change_boundary;
        }
        
        uint32_t zoom = lv_map(offset_abs, 0, text_drag->zoom_change_boundary,
                               text_drag->zoom_max, text_drag->zoom_min);
        uint8_t opa = lv_map(offset_abs, 0, text_drag->opa_change_boundary,
                             text_drag->opa_max, text_drag->opa_min);
        
        lv_img_set_zoom(car_img[i], zoom);
        lv_obj_set_style_img_opa(car_img[i], opa, 0);
    }
}

static void sync_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    
    if (text_roller == NULL || !lv_obj_is_valid(text_roller)) {
        return;
    }
    
    screen_1_t *scr = screen_1_get(&ui_manager);
    if (!scr || !scr->obj || lv_scr_act() != scr->obj) {
        return;
    }
    
    sync_car_to_text();
}

static void text_roller_pressing_cb(lv_event_t *e)
{
    (void)e;
    if (text_roller == NULL || !lv_obj_is_valid(text_roller)) {
        return;
    }
    sync_car_to_text();
}

static void text_roller_released_cb(lv_event_t *e)
{
    (void)e;
    if (text_roller == NULL || !lv_obj_is_valid(text_roller)) {
        return;
    }

    lv_area_t cont_a;
    lv_obj_get_coords(text_roller, &cont_a);
    int32_t cont_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;
    
    uint32_t child_cnt = lv_obj_get_child_cnt(text_roller);
    int min_index = 0;
    int32_t min_value = -1;
    
    for (uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(text_roller, i);
        if (!child || !lv_obj_is_valid(child)) {
            continue;
        }
        
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);
        int32_t child_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
        int32_t diff = LV_ABS(child_center - cont_center);
        
        if (min_value == -1 || diff < min_value) {
            min_value = diff;
            min_index = i;
        }
    }
    
    last_selected_idx = min_index;
    index_img = min_index;
    
    sync_car_to_text();
}

static void text_roller_change_cb(lv_event_t *e)
{
    (void)e;
    if (text_roller == NULL || !lv_obj_is_valid(text_roller)) {
        return;
    }
    
    int32_t selected = lv_obj_drag_get_active_id(text_roller);
    index_img = selected;
    last_selected_idx = selected;
}

void screen_1_roller_created(void) 
{   
    screen_1_t *scr = screen_1_get(&ui_manager);
    if (!scr) {
        return;
    }
    
    if (is_created) {
        if (text_roller && lv_obj_is_valid(text_roller)) {
            lv_obj_del(text_roller);
        }
        text_roller = NULL;
        is_created = 0;
    }
    
    car_img[0] = scr->image_1;
    car_img[1] = scr->image_2;
    car_img[2] = scr->image_3;
    
    for (int i = 0; i < 3; i++) {
        if (car_img[i] && lv_obj_is_valid(car_img[i])) {
            lv_obj_clear_flag(car_img[i], LV_OBJ_FLAG_HIDDEN);
            lv_img_set_src(car_img[i], car_image[i]);
            lv_obj_set_size(car_img[i], CAR_IMG_WIDTH, CAR_IMG_HEIGHT);
            lv_img_set_pivot(car_img[i], CAR_IMG_WIDTH / 2, CAR_IMG_HEIGHT / 2);
            lv_obj_add_flag(car_img[i], LV_OBJ_FLAG_CLICKABLE);
            lv_obj_remove_event_cb_with_user_data(car_img[i], car_click_cb, (void *)(intptr_t)i);
            lv_obj_add_event_cb(car_img[i], car_click_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        }
    }
    
    is_created = 1;
    index_img = last_selected_idx;

    text_roller = lv_obj_drag_create(scr->container_text);
    if (!text_roller) {
        screen_1_roller_cleanup();
        return;
    }
    
    lv_obj_drag_set_dir(text_roller, LV_DRAG_DIR_VER);
    lv_obj_drag_set_infinite(text_roller, true);
    lv_obj_drag_add_label_item(text_roller, mode_text, 3, 180, 55, 
                               fs_droidsansfallback_40, lv_color_hex(0xffffff), 55);
    lv_obj_drag_enable_opa_change(text_roller, true, false);
    lv_obj_drag_set_opa_ratio(text_roller, 255, 180, 150);
    lv_obj_drag_enable_zoom_change(text_roller, true, false);
    lv_obj_drag_set_zoom_ratio(text_roller, 256, 150, 150);
    lv_obj_drag_set_zoom_change_mode(text_roller, LV_DRAG_GRADIENT, LV_DRAG_ZOOM_CENTER);
    lv_obj_drag_set_main_y_offset(text_roller, 50);
    lv_obj_drag_set_active(text_roller, index_img, LV_ANIM_OFF);

    lv_obj_add_event_cb(text_roller, text_roller_released_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(text_roller, text_roller_change_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(text_roller, text_roller_pressing_cb, LV_EVENT_PRESSING, NULL);

    sync_car_to_text();
    
    if (sync_timer == NULL) {
        sync_timer = lv_timer_create(sync_timer_cb, SYNC_PERIOD_MS, NULL);
    } else {
        lv_timer_resume(sync_timer);
    }
    
    hint_animation_init_screen1(scr);
}

/* ========== Screen Switch ========== */

static void switch_to_screen(uint8_t screen_idx)
{
    if (sync_timer) {
        lv_timer_pause(sync_timer);
    }
    
    // Cleanup hint animation
    screen_1_t *scr = screen_1_get(&ui_manager);
    if (scr && scr->label_wait && lv_obj_is_valid(scr->label_wait)) {
        lv_anim_del(scr->label_wait, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_del(scr->label_wait, (lv_anim_exec_xcb_t)lv_obj_set_y);
    }
    
    if (text_roller && lv_obj_is_valid(text_roller)) {
        lv_obj_del(text_roller);
    }
    text_roller = NULL;
    is_created = 0;
    
    for (int i = 0; i < 6; i++) {
        hint_trigger_objs[i] = NULL;
    }
    
    lv_obj_t *act_scr = lv_scr_act();
    if (!screen_is_loading(act_scr)) {
        lv_obj_clean(act_scr);
        
        switch (screen_idx) {
            case 2:
                screen_2_create(&ui_manager);
                lv_scr_load_anim(screen_2_get(&ui_manager)->obj, LV_SCR_LOAD_ANIM_NONE, 0, 0, ui_manager.auto_del);
                break;
            case 3:
                screen_3_create(&ui_manager);
                lv_scr_load_anim(screen_3_get(&ui_manager)->obj, LV_SCR_LOAD_ANIM_NONE, 0, 0, ui_manager.auto_del);
                break;
            case 4:
                screen_4_create(&ui_manager);
                lv_scr_load_anim(screen_4_get(&ui_manager)->obj, LV_SCR_LOAD_ANIM_NONE, 0, 0, ui_manager.auto_del);
                break;
            default:
                break;
        }
    }
}

static void car_click_cb(lv_event_t *e)
{
    int clicked_idx = (int)(intptr_t)lv_event_get_user_data(e);
    
    last_selected_idx = clicked_idx;
    index_img = clicked_idx;
    
    switch (clicked_idx) {
        case 0: switch_to_screen(3); break;
        case 1: switch_to_screen(2); break;
        case 2: switch_to_screen(4); break;
        default: break;
    }
}
/* ========== Cleanup ========== */

void screen_1_roller_cleanup(void)
{
    if (sync_timer) {
        lv_timer_del(sync_timer);
        sync_timer = NULL;
    }
    
    if (text_roller && lv_obj_is_valid(text_roller)) {
        lv_obj_del(text_roller);
    }
    text_roller = NULL;
    
    car_img[0] = NULL;
    car_img[1] = NULL;
    car_img[2] = NULL;
    
    for (int i = 0; i < 6; i++) {
        hint_trigger_objs[i] = NULL;
    }
    
    is_created = 0;
}
