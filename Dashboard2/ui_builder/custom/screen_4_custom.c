/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"
#include <time.h>

/* ========== Macro Definitions ========== */

#define NAV_IMAGE_COUNT     3
#define NAV_INFO_COUNT      3
#define DATA_TIMER_PERIOD   5000
#define TEST_TIMER_PERIOD   10
#define IMAGE_COUNT         5
#define SPEED_MAX           200
#define RPM_MAX             99
#define OFFSET_MAX          176

// Initial coordinates
#define CONTAINER_1_INIT_X      23
#define CONTAINER_1_INIT_Y      381
#define IMAGE_GRADIENT1_INIT_X  4
#define IMAGE_GRADIENT1_INIT_Y  (-176)
#define CONTAINER_2_INIT_X      669
#define CONTAINER_2_INIT_Y      381
#define IMAGE_GRADIENT2_INIT_X  0
#define IMAGE_GRADIENT2_INIT_Y  (-176)

/* ========== Static Data ========== */

// Driving distance data
static const char *s_data_values[] = {"510", "420", "380", "350", "290"};
static const uint8_t s_data_value_count = 5;

// Navigation image paths
static const char *nav_image_paths[NAV_IMAGE_COUNT] = {
    LVGL_IMAGE_PATH(turn/straight2.png),
    LVGL_IMAGE_PATH(turn/left.png),
    LVGL_IMAGE_PATH(turn/right.png)
};

// Navigation hint text
static const char *nav_info_texts[NAV_INFO_COUNT] = {
    "直行进入珠海大桥",
    "左转进入香港路",
    "右转进入澳门大道"
};

// Gear images
static const char *image_array[IMAGE_COUNT] = {
    LVGL_IMAGE_PATH(gear/P.png),
    LVGL_IMAGE_PATH(gear/R.png),
    LVGL_IMAGE_PATH(gear/N.png),
    LVGL_IMAGE_PATH(gear/D.png),
    LVGL_IMAGE_PATH(gear/S.png)
};

/* ========== Static Variables ========== */

// Data management
static uint8_t s_data_index = 0;
static lv_timer_t *data_timer = NULL;

// Navigation info
static lv_obj_t *image_nav = NULL;
static lv_obj_t *label_nav_text = NULL;
static lv_obj_t *label_distance = NULL;

// Speed/RPM coordinate linkage
static lv_obj_t *speed_container = NULL;
static lv_obj_t *speed_image = NULL;
static lv_obj_t *rpm_container = NULL;
static lv_obj_t *rpm_image = NULL;

// Gear rotation
static lv_obj_t *image_obj = NULL;
static lv_timer_t *image_timer = NULL;
static uint8_t image_idx = 0;

// Test simulator
static lv_timer_t *test_timer = NULL;
static uint8_t test_speed = 0;
static uint8_t test_digit_low = 0;
static uint8_t test_digit_high = 0;

/* ========== Function Forward Declarations ========== */

// Utility functions
static lv_obj_t* get_valid_obj(lv_obj_t *obj);

// Data management
static const char* data_manager_get_value(void);
static void data_manager_next_value(void);
static void data_manager_init(void);
static void data_timer_cb(lv_timer_t *timer);

// Navigation info
static void get_nav_info_ui_objects(screen_4_t *scr);
static void nav_info_update_by_index(uint8_t idx);

// Coordinate linkage
static void get_speed_rpm_ui_objects(screen_4_t *scr);
static void update_speed_position(uint16_t speed);
static void update_rpm_position(uint16_t rpm);

// Gear rotation
static void image_rotate_cb(lv_timer_t *timer);
static void image3_slider_init(lv_obj_t *img_obj, uint32_t interval_ms);
static void image3_slider_stop(void);

// Test simulator
static void test_simulator_cb(lv_timer_t *timer);

// Display update
void screen_4_set_speed(uint16_t target);
void screen_4_set_rpm(uint16_t target);
void test_speed_simulator_init_screen4(void);
void screen_4_anim_cleanup(void);

/* ========== Utility Functions ========== */

static lv_obj_t* get_valid_obj(lv_obj_t *obj)
{
    return (obj && lv_obj_is_valid(obj)) ? obj : NULL;
}

/* ========== Data Management ========== */

static const char* data_manager_get_value(void)
{
    return s_data_values[s_data_index];
}

static void data_manager_next_value(void)
{
    s_data_index = (s_data_index + 1) % s_data_value_count;
}

static void data_manager_init(void)
{
    if (data_timer == NULL) {
        data_timer = lv_timer_create(data_timer_cb, DATA_TIMER_PERIOD, NULL);
    }
}

static void data_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    
    data_manager_next_value();
    const char *value = data_manager_get_value();
    uint8_t idx = s_data_index;
    
    screen_4_t *scr = screen_4_get(&ui_manager);
    if (!scr || !scr->obj || lv_scr_act() != scr->obj) {
        return;
    }
    
    if (label_distance && lv_obj_is_valid(label_distance)) {
        lv_label_set_text(label_distance, value);
    }
    
    nav_info_update_by_index(idx);
}

/* ========== Navigation Info ========== */

static void get_nav_info_ui_objects(screen_4_t *scr)
{
    label_distance = get_valid_obj(scr->label_meter);
    label_nav_text = get_valid_obj(scr->label_address);
    image_nav = get_valid_obj(scr->image_turn);
}

static void nav_info_update_by_index(uint8_t idx)
{
    uint8_t nav_idx = idx % NAV_IMAGE_COUNT;
    
    if (image_nav && nav_image_paths[nav_idx]) {
        lv_img_set_src(image_nav, nav_image_paths[nav_idx]);
    }
    
    if (label_nav_text) {
        lv_label_set_text(label_nav_text, nav_info_texts[nav_idx]);
    }
}

/* ========== Coordinate Linkage ========== */

static void get_speed_rpm_ui_objects(screen_4_t *scr)
{
    speed_container = get_valid_obj(scr->container_gradient1);
    speed_image = get_valid_obj(scr->image_gradient1);
    rpm_container = get_valid_obj(scr->container_gradient2);
    rpm_image = get_valid_obj(scr->image_gradient2);
}

static void update_speed_position(uint16_t speed)
{
    if (!speed_container || !speed_image) {
        return;
    }
    
    if (speed > SPEED_MAX) {
        speed = SPEED_MAX;
    }
    
    int offset = (int)(speed * OFFSET_MAX / SPEED_MAX);
    
    lv_obj_set_y(speed_container, CONTAINER_1_INIT_Y - offset);
    lv_obj_set_pos(speed_image, IMAGE_GRADIENT1_INIT_X, IMAGE_GRADIENT1_INIT_Y + offset);
}

static void update_rpm_position(uint16_t rpm)
{
    if (!rpm_container || !rpm_image) {
        return;
    }
    
    if (rpm > RPM_MAX) {
        rpm = RPM_MAX;
    }
    
    int offset = (int)(rpm * OFFSET_MAX / RPM_MAX);
    
    lv_obj_set_y(rpm_container, CONTAINER_2_INIT_Y - offset);
    lv_obj_set_pos(rpm_image, IMAGE_GRADIENT2_INIT_X, IMAGE_GRADIENT2_INIT_Y + offset);
}

/* ========== Display Update ========== */

void screen_4_set_speed(uint16_t target)
{
    screen_4_t *scr = screen_4_get(&ui_manager);
    if (!scr || !scr->label_km) {
        return;
    }
    
    if (target > SPEED_MAX) {
        target = SPEED_MAX;
    }
    
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", target);
    lv_label_set_text(scr->label_km, buf);
    
    update_speed_position(target);
}

void screen_4_set_rpm(uint16_t target)
{
    screen_4_t *scr = screen_4_get(&ui_manager);
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
    
    update_rpm_position(target);
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

static void image3_slider_init(lv_obj_t *img_obj, uint32_t interval_ms)
{
    if (!img_obj) {
        return;
    }
    
    image3_slider_stop();
    
    image_obj = img_obj;
    image_idx = 0;
    
    if (image_array[0]) {
        lv_img_set_src(image_obj, image_array[0]);
    }
    
    image_timer = lv_timer_create(image_rotate_cb, interval_ms, NULL);
}

static void image3_slider_stop(void)
{
    if (image_timer) {
        lv_timer_del(image_timer);
        image_timer = NULL;
    }
    image_obj = NULL;
    image_idx = 0;
}

/* ========== Test Simulator ========== */

static void test_simulator_cb(lv_timer_t *timer)
{
    (void)timer;

    uint16_t rpm = test_digit_high * 10 + test_digit_low;
    
    screen_4_set_speed(test_speed);
    screen_4_set_rpm(rpm);
    
    // Speed update
    if (test_speed < 50) {
        test_speed += 2;
    } else if (test_speed < 100) {
        test_speed += 5;
    } else if (test_speed < 150) {
        test_speed += 2;
    } else if (test_speed < SPEED_MAX) {
        test_speed += 5;
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

void test_speed_simulator_init_screen4(void)
{
    if (test_timer) {
        return;
    }
    
    test_timer = lv_timer_create(test_simulator_cb, TEST_TIMER_PERIOD, NULL);

    screen_4_t *scr = screen_4_get(&ui_manager);
    if (scr && scr->image_P) {
        image3_slider_init(scr->image_P, 3000);
    }
    
    data_manager_init();
    get_nav_info_ui_objects(scr);
    get_speed_rpm_ui_objects(scr);
    
    if (label_distance) {
        lv_label_set_text(label_distance, data_manager_get_value());
    }
    
    nav_info_update_by_index(s_data_index);
    
    update_speed_position(0);
    update_rpm_position(0);
}

/* ========== Cleanup ========== */

void screen_4_anim_cleanup(void)
{
    if (test_timer) {
        lv_timer_del(test_timer);
        test_timer = NULL;
        test_speed = 0;
        test_digit_low = 0;
        test_digit_high = 0;
    }
    
    image3_slider_stop();
    
    if (data_timer) {
        lv_timer_del(data_timer);
        data_timer = NULL;
    }
    
    image_nav = NULL;
    label_nav_text = NULL;
    label_distance = NULL;
    speed_container = NULL;
    speed_image = NULL;
    rpm_container = NULL;
    rpm_image = NULL;
}
