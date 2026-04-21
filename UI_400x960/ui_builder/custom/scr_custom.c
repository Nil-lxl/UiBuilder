#include "custom.h"
#include "ui_objects.h"

static screen_t *scr;

static lv_style_t temp_arc_style;
static lv_style_t temp_arc_bg_style;

static lv_style_t fan_arc_style;
static lv_style_t fan_arc_bg_style;

static lv_style_t humidity_arc_style;
static lv_style_t humidity_arc_bg_style;

static void temp_arc_timer_cb(lv_timer_t *timer);
static void temp_status_timer_cb(lv_timer_t *timer);
static void param_arc_timer_cb(lv_timer_t *timer);
static void param_label_timer_cb(lv_timer_t *timer);

static void chart_add_data(lv_timer_t *timer) {
    lv_obj_t *chart = lv_timer_get_user_data(timer);
    lv_chart_series_t *ser = lv_chart_get_series_next(chart, NULL);

    int value = (int)((atof(lv_label_get_text(scr->temp_label))) * 10);

    lv_chart_set_next_value(chart, ser, value);

    uint16_t p = lv_chart_get_point_count(chart);
    uint16_t s = lv_chart_get_x_start_point(chart, ser);
    int32_t *a = lv_chart_get_y_array(chart, ser);

    a[(s + 1) % p] = LV_CHART_POINT_NONE;
    a[(s + 2) % p] = LV_CHART_POINT_NONE;
    a[(s + 2) % p] = LV_CHART_POINT_NONE;

    lv_chart_refresh(chart);
}

void screen_custom_load_start(void) {
    scr = screen_get(&ui_manager);

    /* 圆弧指示条样式 */
    lv_style_init(&temp_arc_style);
    lv_style_set_arc_width(&temp_arc_style, 40);
    lv_style_set_arc_opa(&temp_arc_style, 255);
    lv_style_set_arc_color(&temp_arc_style, lv_color_hex(0x63aef4));
    // lv_style_set_arc_opa(&temp_arc_style, 200);
    lv_style_set_arc_rounded(&temp_arc_style, 10);

    lv_style_init(&temp_arc_bg_style);

    lv_style_init(&fan_arc_style);
    lv_style_set_arc_color(&fan_arc_style, lv_color_hex(0xff5580));
    lv_style_set_arc_rounded(&fan_arc_style, 0);

    lv_style_init(&fan_arc_bg_style);
    lv_style_set_arc_rounded(&fan_arc_bg_style, 0);

    lv_style_init(&humidity_arc_style);
    lv_style_set_arc_color(&humidity_arc_style, lv_color_hex(0x00aa7d));
    lv_style_set_arc_rounded(&humidity_arc_style, 0);

    lv_style_init(&humidity_arc_bg_style);
    lv_style_set_arc_rounded(&humidity_arc_bg_style, 0);


    /* 添加样式到圆弧对象 */
    lv_obj_t *temp_arc = scr->temp_arc;
    lv_obj_add_style(temp_arc, &temp_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(temp_arc, &temp_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(temp_arc, NULL, LV_PART_KNOB);

    lv_obj_t *fan_arc = scr->fan_arc;
    lv_obj_add_style(fan_arc, &fan_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(fan_arc, &fan_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(fan_arc, NULL, LV_PART_KNOB);

    lv_obj_t *hum_arc = scr->humidity_arc;
    lv_obj_add_style(hum_arc, &humidity_arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(hum_arc, &humidity_arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(hum_arc, NULL, LV_PART_KNOB);


    /* 曲线图对象创建 */
    lv_obj_t *chart = scr->temp_chart;
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_point_count(chart, 80);
    lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
    lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_hex(0xff0080), LV_CHART_AXIS_PRIMARY_Y);

    /* 定时器创建 */
    lv_timer_create(temp_arc_timer_cb, 10, NULL);
    lv_timer_create(param_arc_timer_cb, 10, NULL);
    lv_timer_create(param_label_timer_cb, 100, NULL);
    lv_timer_create(temp_status_timer_cb, 50, NULL);
    // lv_timer_create(chart_add_data, 500, chart);
}

static int temp_arc_direct = 0;
static int start_degree = 0;
static int end_degree = 120;
static void temp_arc_timer_cb(lv_timer_t *timer) {
    lv_obj_t *temp_arc = scr->temp_arc;

    start_degree += 2;
    end_degree += 2;

    if (end_degree > 360) {
        start_degree = (start_degree + 2) % 360;
        end_degree = (end_degree + 2) % 360;

    }

    lv_arc_set_start_angle(temp_arc, start_degree);
    lv_arc_set_end_angle(temp_arc, end_degree);
}


static int temp_direct = 0;
static int temp_value = 160;
static void temp_status_timer_cb(lv_timer_t *timer) {
    lv_obj_t *temp_arc = scr->temp_arc;

    if (temp_direct == 0) {
        temp_value++;
        if (temp_value > 250) { //大于25度，显示加热状态
            lv_obj_set_style_arc_color(temp_arc, lv_color_hex(0xff6000), LV_PART_INDICATOR);
            lv_img_set_src(scr->cold_hot, LVGL_IMAGE_PATH(hot_40x40.png));
            lv_obj_set_style_img_recolor(scr->cold_hot, lv_color_hex(0xff6000), LV_PART_MAIN | LV_STATE_DEFAULT);
            // lv_obj_set_style_text_color(scr->temp_label, lv_color_hex(0xff6000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(scr->temp_status, "Heating");
            lv_obj_set_style_shadow_color(scr->temp_cont, lv_color_hex(0xff6000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        if (temp_value > 350) {
            temp_value = 350;
            temp_direct = 1;
        }
    } else {
        temp_value--;
        if (temp_value < 250) { //小于25度，显示制冷状态
            lv_obj_set_style_arc_color(temp_arc, lv_color_hex(0x63aef4), LV_PART_INDICATOR);
            lv_img_set_src(scr->cold_hot, LVGL_IMAGE_PATH(cold_40x40.png));
            lv_obj_set_style_img_recolor(scr->cold_hot, lv_color_hex(0x55aaff), LV_PART_MAIN | LV_STATE_DEFAULT);
            // lv_obj_set_style_text_color(scr->temp_label, lv_color_hex(0x55aaff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(scr->temp_status, "Colding");
            lv_obj_set_style_shadow_color(scr->temp_cont, lv_color_hex(0x55aaff), LV_PART_MAIN | LV_STATE_DEFAULT);

        }
        if (temp_value < 160) {
            temp_value = 160;
            temp_direct = 0;
        }
    }

    float label_value = (float)(temp_value / 10.0);

    char label_value_str[16];
    snprintf(label_value_str, sizeof(label_value_str), "%02.1f", label_value);
    lv_label_set_text_fmt(scr->temp_label, "%s", label_value_str);
    // lv_label_set_text_fmt(scr->temp_label, "%02.1f", label_value);

}


static int fan_direct = 0;
static int fan_value = 50;

static int humidity_direct = 0;
static int humidity_value = 30;

static void param_arc_timer_cb(lv_timer_t *timer) {
    lv_obj_t *fan_arc = scr->fan_arc;
    lv_obj_t *hum_arc = scr->humidity_arc;
    lv_arc_set_value(fan_arc, fan_value);
    lv_arc_set_value(hum_arc, humidity_value);

    if (fan_direct == 0) {
        fan_value++;
        if (fan_value > 100) {
            fan_value = 100;
            fan_direct = 1;
        }
    } else {
        fan_value--;
        if (fan_value < 0) {
            fan_value = 0;
            fan_direct = 0;
        }
    }

    if (humidity_direct == 0) {
        humidity_value++;
        if (humidity_value > 100) {
            humidity_value = 100;
            humidity_direct = 1;
        }
    } else {
        humidity_value--;
        if (humidity_value < 0) {
            humidity_value = 0;
            humidity_direct = 0;
        }
    }
    return;

}

static void param_label_timer_cb(lv_timer_t *timer) {
    int fan_value = lv_arc_get_value(scr->fan_arc);
    int humidity_value = lv_arc_get_value(scr->humidity_arc);

    lv_label_set_text_fmt(scr->label_7, "%d%%", fan_value);
    lv_label_set_text_fmt(scr->label_8, "%d%%", humidity_value);
}
