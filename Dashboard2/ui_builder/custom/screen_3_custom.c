/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"
#include <time.h>

/* ========== Macro Definitions ========== */

// Image positions
#define LEFT_IMAGE_X        44
#define LEFT_IMAGE_Y        276
#define RIGHT_IMAGE_X       830
#define RIGHT_IMAGE_Y       278

// Animation config
#define MAX_ANIM_OBJS       7
#define ANIM_FADE_TIME      800
#define ANIM_STAY_TIME      600
#define ANIM_INTERVAL       ((ANIM_FADE_TIME * 2 + ANIM_STAY_TIME) / MAX_ANIM_OBJS)

// Value limits
#define SPEED_MAX           200
#define RPM_MAX             99
#define RPM_LEVEL_DIV       11
#define SPEED_LEVEL_DIV     22
#define IMAGE_COUNT         5
#define TEST_TIMER_PERIOD   50
#define SLIDER_INTERVAL     3000
#define LEVEL_IMAGE_COUNT   9

/* ========== Type Definitions ========== */

typedef struct {
    lv_coord_t x;
    lv_coord_t y;
} img_pos_t;

typedef struct {
    lv_obj_t *obj;
    uint16_t idx;
} simple_anim_t;

/* ========== Static Data ========== */

// Left RPM images
static const char *left_image_paths[LEVEL_IMAGE_COUNT] = {
    LVGL_IMAGE_PATH(l1.png), LVGL_IMAGE_PATH(l2.png), LVGL_IMAGE_PATH(l3.png),
    LVGL_IMAGE_PATH(l4.png), LVGL_IMAGE_PATH(l5.png), LVGL_IMAGE_PATH(l6.png),
    LVGL_IMAGE_PATH(l7.png), LVGL_IMAGE_PATH(l8.png), LVGL_IMAGE_PATH(l9.png)
};

// Right speed images
static const char *right_image_paths[LEVEL_IMAGE_COUNT] = {
    LVGL_IMAGE_PATH(r1.png), LVGL_IMAGE_PATH(r2.png), LVGL_IMAGE_PATH(r3.png),
    LVGL_IMAGE_PATH(r4.png), LVGL_IMAGE_PATH(r5.png), LVGL_IMAGE_PATH(r6.png),
    LVGL_IMAGE_PATH(r7.png), LVGL_IMAGE_PATH(r8.png), LVGL_IMAGE_PATH(r9.png)
};

// Gear images
static const char *image_array[IMAGE_COUNT] = {
    LVGL_IMAGE_PATH(gear/P.png), LVGL_IMAGE_PATH(gear/R.png),
    LVGL_IMAGE_PATH(gear/N.png), LVGL_IMAGE_PATH(gear/D.png),
    LVGL_IMAGE_PATH(gear/S.png)
};

// Breathing animation positions
static const img_pos_t original_positions[MAX_ANIM_OBJS] = {
    {352, 238}, {393, 205}, {434, 180}, {475, 211},
    {516, 132}, {557, 186}, {598, 205}
};

/* ========== Static Variables ========== */

// Left/right image control
static lv_obj_t *left_image_obj = NULL;
static lv_obj_t *right_image_obj = NULL;
static int8_t last_rpm_level = -1;
static int8_t last_speed_level = -1;

// Gear rotation
static lv_obj_t *image_obj = NULL;
static lv_timer_t *image_timer = NULL;
static uint8_t image_idx = 0;

// Breathing animation
static simple_anim_t s_anim_objs[MAX_ANIM_OBJS] = {0};
static uint8_t s_anim_active = 0;

// Test simulator
static lv_timer_t *test_timer = NULL;
static uint8_t test_speed = 0;
static uint8_t test_digit_low = 0;
static uint8_t test_digit_high = 0;

/* ========== Function Forward Declarations ========== */

// Utility functions
// static lv_obj_t* get_valid_obj(lv_obj_t *obj);

// Left/right images
static void lr_images_init(lv_obj_t *parent);
static void lr_images_stop(void);
static void set_rpm_image(uint16_t rpm);
static void set_speed_image(uint16_t speed);

// Gear rotation
static void image_rotate_cb(lv_timer_t *timer);
static void image2_slider_init(lv_obj_t *img_obj, uint32_t interval_ms);
static void image2_slider_stop(void);

// Breathing animation
static void simple_breath_cb(void *var, int32_t v);
static void dynamic_images_stop(void);

// Test simulator
static void test_simulator_cb(lv_timer_t *timer);

// Display update
void screen_3_set_speed(uint16_t target);
void screen_3_set_rpm(uint16_t target);
void test_speed_simulator_init_screen3(void);
void screen_3_anim_cleanup(void);

/* ========== Utility Functions ========== */

// static lv_obj_t* get_valid_obj(lv_obj_t *obj)
// {
//     return (obj && lv_obj_is_valid(obj)) ? obj : NULL;
// }

/* ========== Left/Right Image Control ========== */

static void lr_images_init(lv_obj_t *parent)
{
    left_image_obj = lv_img_create(parent);
    if (left_image_obj) {
        lv_obj_set_pos(left_image_obj, LEFT_IMAGE_X, LEFT_IMAGE_Y);
        lv_obj_set_size(left_image_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_add_flag(left_image_obj, LV_OBJ_FLAG_HIDDEN);
    }
    
    right_image_obj = lv_img_create(parent);
    if (right_image_obj) {
        lv_obj_set_pos(right_image_obj, RIGHT_IMAGE_X, RIGHT_IMAGE_Y);
        lv_obj_set_size(right_image_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_add_flag(right_image_obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void lr_images_stop(void)
{
    if (left_image_obj) {
        lv_obj_del(left_image_obj);
        left_image_obj = NULL;
    }
    if (right_image_obj) {
        lv_obj_del(right_image_obj);
        right_image_obj = NULL;
    }
}

static void set_rpm_image(uint16_t rpm)
{
    if (!left_image_obj) {
        return;
    }
    
    if (rpm > RPM_MAX) {
        rpm = 0;
    }
    
    if (rpm == 0) {
        lv_obj_add_flag(left_image_obj, LV_OBJ_FLAG_HIDDEN);
        last_rpm_level = 0;
        return;
    }
    
    uint8_t level = (rpm - 1) / RPM_LEVEL_DIV + 1;
    if (level > LEVEL_IMAGE_COUNT) {
        level = LEVEL_IMAGE_COUNT;
    }
    
    if (level != last_rpm_level) {
        lv_img_set_src(left_image_obj, left_image_paths[level - 1]);
        last_rpm_level = level;
    }
    
    lv_obj_clear_flag(left_image_obj, LV_OBJ_FLAG_HIDDEN);
}

static void set_speed_image(uint16_t speed)
{
    if (!right_image_obj) {
        return;
    }
    
    if (speed > SPEED_MAX) {
        speed = SPEED_MAX;
    }
    
    if (speed == 0) {
        lv_obj_add_flag(right_image_obj, LV_OBJ_FLAG_HIDDEN);
        last_speed_level = 0;
        return;
    }
    
    uint8_t level = (speed - 1) / SPEED_LEVEL_DIV + 1;
    if (level > LEVEL_IMAGE_COUNT) {
        level = LEVEL_IMAGE_COUNT;
    }
    
    if (level != last_speed_level) {
        lv_img_set_src(right_image_obj, right_image_paths[level - 1]);
        last_speed_level = level;
    }
    
    lv_obj_clear_flag(right_image_obj, LV_OBJ_FLAG_HIDDEN);
}

/* ========== Display Update ========== */

void screen_3_set_speed(uint16_t target)
{
    screen_3_t *scr = screen_3_get(&ui_manager);
    if (!scr || !scr->label_km) {
        return;
    }
    
    if (target > SPEED_MAX) {
        target = SPEED_MAX;
    }
    
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", target);
    lv_label_set_text(scr->label_km, buf);
    
    set_speed_image(target);
}

void screen_3_set_rpm(uint16_t target)
{
    screen_3_t *scr = screen_3_get(&ui_manager);
    if (!scr) {
        return;
    }
    
    if (target > RPM_MAX) {
        target = RPM_MAX;
    }
    
    uint8_t left_digit = target / 10;
    uint8_t right_digit = target % 10;
    char buf[8];
    
    if (scr->label_left) {
        lv_snprintf(buf, sizeof(buf), "%d", left_digit);
        lv_label_set_text(scr->label_left, buf);
    }
    
    if (scr->label_right) {
        lv_snprintf(buf, sizeof(buf), "%d", right_digit);
        lv_label_set_text(scr->label_right, buf);
    }
    
    set_rpm_image(target);
}

/* ========== Gear Rotation ========== */

static void image_rotate_cb(lv_timer_t *timer)
{
    (void)timer;
    
    if (!image_obj || !image_array[image_idx]) {
        return;
    }
    
    lv_img_set_src(image_obj, image_array[image_idx]);
    image_idx = (image_idx + 1) % IMAGE_COUNT;
}

static void image2_slider_init(lv_obj_t *img_obj, uint32_t interval_ms)
{
    if (!img_obj) {
        return;
    }
    
    image2_slider_stop();
    
    image_obj = img_obj;
    image_idx = 0;
    
    if (image_array[0]) {
        lv_img_set_src(image_obj, image_array[0]);
    }
    
    image_timer = lv_timer_create(image_rotate_cb, interval_ms, NULL);
}

static void image2_slider_stop(void)
{
    if (image_timer) {
        lv_timer_del(image_timer);
        image_timer = NULL;
    }
    image_obj = NULL;
    image_idx = 0;
}

/* ========== Breathing Animation ========== */

static void simple_breath_cb(void *var, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)var, v, 0);
}

void dynamic_images_init(void)
{
    dynamic_images_stop();
    
    screen_3_t *scr = screen_3_get(&ui_manager);
    if (!scr) {
        return;
    }
    
    lv_obj_t *objs[MAX_ANIM_OBJS] = {
        scr->image_bar1, scr->image_bar2, scr->image_bar3,
        scr->image_bar4, scr->image_bar5, scr->image_bar6,
        scr->image_bar7
    };
    
    for (int i = 0; i < MAX_ANIM_OBJS; i++) {
        s_anim_objs[i].obj = objs[i];
        s_anim_objs[i].idx = i;
        
        if (!s_anim_objs[i].obj) {
            continue;
        }
        
        lv_obj_set_pos(s_anim_objs[i].obj,
                      original_positions[s_anim_objs[i].idx].x,
                      original_positions[s_anim_objs[i].idx].y);
        lv_obj_clear_flag(s_anim_objs[i].obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_opa(s_anim_objs[i].obj, LV_OPA_0, 0);
        
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, s_anim_objs[i].obj);
        lv_anim_set_exec_cb(&a, simple_breath_cb);
        lv_anim_set_values(&a, LV_OPA_0, LV_OPA_COVER);
        lv_anim_set_time(&a, ANIM_FADE_TIME);
        lv_anim_set_playback_time(&a, ANIM_FADE_TIME);
        lv_anim_set_playback_delay(&a, ANIM_STAY_TIME);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_delay(&a, i * ANIM_INTERVAL);
        
        lv_anim_start(&a);
    }
    
    s_anim_active = 1;
}

static void dynamic_images_stop(void)
{
    if (!s_anim_active) {
        return;
    }
    
    for (int i = 0; i < MAX_ANIM_OBJS; i++) {
        if (s_anim_objs[i].obj) {
            lv_anim_del(s_anim_objs[i].obj, simple_breath_cb);
            lv_obj_set_style_opa(s_anim_objs[i].obj, LV_OPA_COVER, 0);
        }
        s_anim_objs[i].obj = NULL;
    }
    
    s_anim_active = 0;
}

/* ========== Test Simulator ========== */

static void test_simulator_cb(lv_timer_t *timer)
{
    (void)timer;
    
    uint16_t rpm = test_digit_high * 10 + test_digit_low;
    
    screen_3_set_speed(test_speed);
    screen_3_set_rpm(rpm);
    
    // Speed update
    if (test_speed < 50) {
        test_speed += 5;
    } else if (test_speed < 100) {
        test_speed += 10;
    } else if (test_speed < 150) {
        test_speed += 5;
    } else if (test_speed < SPEED_MAX) {
        test_speed += 10;
    } else {
        test_speed = 0;
    }

    // RPM update
    uint16_t rpm_temp = rpm;
    if (rpm_temp < 25) {
        rpm_temp += 1;
    } else if (rpm_temp < 50) {
        rpm_temp += 2;
    } else if (rpm_temp < 75) {
        rpm_temp += 1;
    } else if (rpm_temp < 100) {
        rpm_temp += 2;
    } else {
        rpm_temp = 0;
    }
    
    test_digit_high = rpm_temp / 10;
    test_digit_low = rpm_temp % 10;
}

void test_speed_simulator_init_screen3(void)
{
    if (test_timer) {
        return;
    }
    
    screen_3_t *scr = screen_3_get(&ui_manager);
    if (!scr) {
        return;
    }
    
    lr_images_init(lv_scr_act());
    
    test_timer = lv_timer_create(test_simulator_cb, TEST_TIMER_PERIOD, NULL);

    if (scr->image_P) {
        image2_slider_init(scr->image_P, SLIDER_INTERVAL);
    }
    
    dynamic_images_init();
}

/* ========== Cleanup ========== */

void screen_3_anim_cleanup(void)
{
    if (test_timer) {
        lv_timer_del(test_timer);
        test_timer = NULL;
        test_speed = 0;
        test_digit_low = 0;
        test_digit_high = 0;
        last_speed_level = -1;
        last_rpm_level = -1;
    }
    
    image2_slider_stop();
    dynamic_images_stop();
    lr_images_stop();
}
