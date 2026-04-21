#include "custom.h"
#include "ui_objects.h"


static screen_t *scr;

static lv_style_t speed_arc_style;
static lv_style_t speed_arc_bg_style;

static lv_style_t power_arc_style;
static lv_style_t power_arc_bg_style;

static lv_style_t temp_arc_style;
static lv_style_t temp_arc_bg_style;

static lv_style_t battemp_arc_style;
static lv_style_t battemp_arc_bg_style;

static lv_style_t battery_arc_style;
static lv_style_t battery_arc_bg_style;

static lv_timer_t *arc_timer = NULL;
static lv_timer_t *battery_timer = NULL;

static lv_timer_t *label_timer = NULL;
static lv_timer_t *battery_label_timer = NULL;

static void arc_timer_cb(lv_timer_t *timer);
static void battery_timer_cb(lv_timer_t *timer);
static void label_timer_cb(lv_timer_t *timer);
static void battery_label_timer_cb(lv_timer_t *timer);

static void arc_style_init() {
    /* 圆弧背景样式 */
    lv_style_init(&speed_arc_bg_style);
    lv_style_set_arc_rounded(&speed_arc_bg_style, 0);
    lv_style_set_arc_width(&speed_arc_bg_style, 30);
    lv_style_set_arc_opa(&speed_arc_bg_style, LV_OPA_30);
    lv_style_set_arc_color(&speed_arc_bg_style, lv_palette_main(LV_PALETTE_CYAN));

    lv_style_init(&power_arc_bg_style);
    lv_style_set_arc_rounded(&power_arc_bg_style, 0);
    lv_style_set_arc_width(&power_arc_bg_style, 10);
    lv_style_set_arc_opa(&power_arc_bg_style, LV_OPA_30);
    lv_style_set_arc_color(&power_arc_bg_style, lv_color_hex(0xff66ff));

    lv_style_init(&temp_arc_bg_style);
    lv_style_set_arc_rounded(&temp_arc_bg_style, 0);
    lv_style_set_arc_width(&temp_arc_bg_style, 10);
    lv_style_set_arc_opa(&temp_arc_bg_style, LV_OPA_30);
    lv_style_set_arc_color(&temp_arc_bg_style, lv_color_hex(0x9575ff));

    lv_style_init(&battemp_arc_bg_style);
    lv_style_set_arc_rounded(&battemp_arc_bg_style, 0);
    lv_style_set_arc_width(&battemp_arc_bg_style, 10);
    lv_style_set_arc_opa(&battemp_arc_bg_style, LV_OPA_30);
    lv_style_set_arc_color(&battemp_arc_bg_style, lv_color_hex(0xfd5454));

    lv_style_init(&battery_arc_bg_style);
    lv_style_set_arc_rounded(&battery_arc_bg_style, 0);
    lv_style_set_arc_width(&battery_arc_bg_style, 10);
    lv_style_set_arc_opa(&battery_arc_bg_style, LV_OPA_30);
    lv_style_set_arc_color(&battery_arc_bg_style, lv_color_hex(0x00ff7b));

    /* 圆弧指示器样式 */
    lv_style_init(&speed_arc_style);
    lv_style_set_arc_rounded(&speed_arc_style, 0);
    lv_style_set_arc_width(&speed_arc_style, 30);
    lv_style_set_arc_color(&speed_arc_style, lv_palette_main(LV_PALETTE_CYAN));

    lv_style_init(&power_arc_style);
    lv_style_set_arc_rounded(&power_arc_style, 0);
    lv_style_set_arc_width(&power_arc_style, 10);
    lv_style_set_arc_color(&power_arc_style, lv_color_hex(0xff66ff));

    lv_style_init(&temp_arc_style);
    lv_style_set_arc_rounded(&temp_arc_style, 0);
    lv_style_set_arc_width(&temp_arc_style, 10);
    lv_style_set_arc_color(&temp_arc_style, lv_color_hex(0x9575ff));

    lv_style_init(&battemp_arc_style);
    lv_style_set_arc_rounded(&battemp_arc_style, 0);
    lv_style_set_arc_width(&battemp_arc_style, 10);
    lv_style_set_arc_color(&battemp_arc_style, lv_color_hex(0xfd5454));

    lv_style_init(&battery_arc_style);
    lv_style_set_arc_rounded(&battery_arc_style, 0);
    lv_style_set_arc_width(&battery_arc_style, 10);
    lv_style_set_arc_color(&battery_arc_style, lv_color_hex(0x00ff7b));
}

static void add_data(lv_timer_t *timer) {
    lv_obj_t *chart = lv_timer_get_user_data(timer);
    lv_chart_series_t *ser = lv_chart_get_series_next(chart, NULL);
    lv_chart_set_next_value(chart, ser, lv_rand(10, 90));

    uint16_t p = lv_chart_get_point_count(chart);
    uint16_t s = lv_chart_get_x_start_point(chart, ser);
    int32_t *a = lv_chart_get_y_array(chart, ser);

    a[(s + 1) % p] = LV_CHART_POINT_NONE;
    a[(s + 2) % p] = LV_CHART_POINT_NONE;
    a[(s + 2) % p] = LV_CHART_POINT_NONE;

    lv_chart_refresh(chart);
}


void screen_custom_load_start() {
    scr = screen_get(&ui_manager);

    arc_style_init();
    /* 圆弧对象创建 */
    lv_obj_t *speed_arc = scr->speed_arc;
    lv_obj_add_style(speed_arc, &speed_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(speed_arc, &speed_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(speed_arc, NULL, LV_PART_KNOB);

    lv_obj_t *power_arc = scr->power_arc;
    lv_obj_add_style(power_arc, &power_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(power_arc, &power_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(power_arc, NULL, LV_PART_KNOB);

    lv_obj_t *temp_arc = scr->temp_arc;
    lv_obj_add_style(temp_arc, &temp_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(temp_arc, &temp_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(temp_arc, NULL, LV_PART_KNOB);

    lv_obj_t *battemp_arc = scr->battemp_arc;
    lv_obj_add_style(battemp_arc, &battemp_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(battemp_arc, &battemp_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(battemp_arc, NULL, LV_PART_KNOB);

    lv_obj_t *battery_arc = scr->bettery_arc;
    lv_obj_add_style(battery_arc, &battery_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(battery_arc, &battery_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(battery_arc, NULL, LV_PART_KNOB);

    arc_timer = lv_timer_create(arc_timer_cb, 10, 0);
    battery_timer = lv_timer_create(battery_timer_cb, 100, 0);
    label_timer = lv_timer_create(label_timer_cb, 60, 0);
    // battery_label_timer = lv_timer_create(battery_label_timer_cb, 100, 0);

    lv_obj_t *speed_chart = scr->chart_1;
    lv_chart_set_update_mode(speed_chart, LV_CHART_UPDATE_MODE_CIRCULAR);
    lv_chart_set_point_count(speed_chart, 60);
    lv_obj_set_style_size(speed_chart, 0, 0, LV_PART_INDICATOR);
    lv_chart_series_t *ser = lv_chart_add_series(speed_chart, lv_color_hex(0x47ffff), LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0;i < 60;i++) {
        lv_chart_set_next_value(speed_chart, ser, lv_rand(10, 90));
    }
    lv_timer_create(add_data, 100, speed_chart);

}

void screen_custom_unload_start() {

}

static int speed_direct = 0;
static int speed_value = 0;

static int power_direct = 0;
static int power_value = 50;

static int temp_direct = 0;
static int temp_value = 70;

static int battemp_direct = 0;
static int battemp_value = 20;

static void arc_timer_cb(lv_timer_t *timer) {
    lv_obj_t *speed_arc = scr->speed_arc;
    lv_obj_t *power_arc = scr->power_arc;
    lv_obj_t *temp_arc = scr->temp_arc;
    lv_obj_t *battemp_arc = scr->battemp_arc;

    lv_arc_set_value(speed_arc, speed_value);
    lv_arc_set_value(power_arc, power_value);
    lv_arc_set_value(temp_arc, temp_value);
    lv_arc_set_value(battemp_arc, battemp_value);

    if (speed_direct == 0) {
        speed_value++;
        if (speed_value > 130) {
            speed_value = 130;
            speed_direct = 1;
        }
    } else {
        speed_value--;
        if (speed_value < 0) {
            speed_value = 0;
            speed_direct = 0;
        }
    }

    if (power_direct == 0) {
        power_value++;
        if (power_value > 100) {
            power_value = 100;
            power_direct = 1;
        }
    } else {
        power_value--;
        if (power_value < 0) {
            power_value = 0;
            power_direct = 0;
        }
    }

    if (temp_direct == 0) {
        temp_value++;
        if (temp_value > 100) {
            temp_value = 100;
            temp_direct = 1;
        }
    } else {
        temp_value--;
        if (temp_value < 0) {
            temp_value = 0;
            temp_direct = 0;
        }
    }

    if (battemp_direct == 0) {
        battemp_value++;
        if (battemp_value > 100) {
            battemp_value = 100;
            battemp_direct = 1;
        }
    } else {
        battemp_value--;
        if (battemp_value < 0) {
            battemp_value = 0;
            battemp_direct = 0;
        }
    }
    return;

}

static int battery_direct = 0;
static int battery_value = 80;
static void battery_timer_cb(lv_timer_t *timer) {
    lv_obj_t *battery_arc = scr->bettery_arc;

    lv_arc_set_value(battery_arc, battery_value);
    if (battery_direct == 0) {
        battery_value++;
        if (battery_value > 100) {
            battery_value = 100;
            battery_direct = 1;
        }
    } else {
        battery_value--;
        if (battery_value < 0) {
            battery_value = 0;
            battery_direct = 0;
        }
    }
    return;
}

static void label_timer_cb(lv_timer_t *timer) {
    char data_str[128];
    int speed_num = speed_value;
    lv_label_set_text_fmt(scr->speed_label, "%02d", speed_num);

    int power_num = power_value;
    lv_label_set_text_fmt(scr->power_num, "%d%%", power_num);

    int temp_num = temp_value;
    lv_label_set_text_fmt(scr->temp_num, "%d%%", temp_num);

    int battemp_num = battemp_value;
    lv_label_set_text_fmt(scr->battemp_num, "%d%%", battemp_num);

    int battery_num = battery_value;
    lv_label_set_text_fmt(scr->battery_num, "%d%%", battery_num);

}

