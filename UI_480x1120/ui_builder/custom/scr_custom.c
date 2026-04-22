#include "custom.h"
#include "ui_objects.h"

static screen_t *scr;

static lv_style_t o2_bar_style;
static lv_style_t o2_bar_bg_style;

static lv_style_t air_bar_style;
static lv_style_t air_bar_bg_style;

static lv_style_t arc_bg_style;
static lv_style_t arc1_indicator_style;
static lv_style_t arc2_indicator_style;

static void anim_timer_cb(lv_timer_t *timer);
static void num_timer_cb(lv_timer_t *timer);

static void set_bar(void *bar, int32_t value) {
    lv_bar_set_value(bar, value, LV_ANIM_ON);
}

void screen_custom_load_start(void) {
    scr = screen_get(&ui_manager);

    /* 进度条样式 */
    lv_style_init(&o2_bar_style);
    lv_style_set_radius(&o2_bar_style, 0);
    lv_style_set_bg_color(&o2_bar_style, lv_color_hex(0xf78511));

    lv_style_init(&o2_bar_bg_style);
    lv_style_set_radius(&o2_bar_bg_style, 5);

    lv_style_init(&air_bar_style);
    lv_style_set_radius(&air_bar_style, 0);
    lv_style_set_bg_color(&air_bar_style, lv_color_hex(0x2570ee));

    lv_style_init(&air_bar_bg_style);
    lv_style_set_radius(&air_bar_bg_style, 5);

    /* 圆弧样式 */
    lv_style_init(&arc_bg_style);
    lv_style_set_arc_color(&arc_bg_style, lv_color_hex(0x00aaff));
    lv_style_set_arc_opa(&arc_bg_style, 255);

    lv_style_init(&arc1_indicator_style);
    lv_style_set_arc_width(&arc1_indicator_style, 40);
    lv_style_set_arc_rounded(&arc1_indicator_style, 0);
    lv_style_set_arc_color(&arc1_indicator_style, lv_color_hex(0xf78511));

    lv_style_init(&arc2_indicator_style);
    lv_style_set_arc_width(&arc2_indicator_style, 40);
    lv_style_set_arc_rounded(&arc2_indicator_style, 0);
    lv_style_set_arc_color(&arc2_indicator_style, lv_color_hex(0x4eb035));

    lv_obj_t *o2_bar = scr->o2_bar;
    lv_obj_add_style(o2_bar, &o2_bar_style, LV_PART_INDICATOR);
    lv_obj_add_style(o2_bar, &o2_bar_bg_style, LV_PART_MAIN);

    lv_obj_t *air_bar = scr->air_bar;
    lv_obj_add_style(air_bar, &air_bar_style, LV_PART_INDICATOR);
    lv_obj_add_style(air_bar, &air_bar_bg_style, LV_PART_MAIN);

    lv_obj_t *arc1 = scr->arc_1;
    lv_obj_add_style(arc1, &arc1_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(arc1, &arc_bg_style, LV_PART_MAIN);
    lv_obj_remove_style(arc1, NULL, LV_PART_KNOB);

    lv_obj_t *arc2 = scr->arc_2;
    lv_obj_add_style(arc2, &arc2_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(arc2, &arc_bg_style, LV_PART_MAIN);
    lv_obj_remove_style(arc2, NULL, LV_PART_KNOB);


    lv_timer_create(anim_timer_cb, 10, NULL);
    lv_timer_create(num_timer_cb, 100, NULL);
}

static int o2_direct = 0;
static int o2_value = 30;

static int air_direct = 0;
static int air_value = 70;

static int arc1_direct = 0;
static int arc1_value = 10;

static int arc2_direct = 0;
static int arc2_value = 50;

static void anim_timer_cb(lv_timer_t *timer) {
    lv_obj_t *o2_bar = scr->o2_bar;
    lv_obj_t *air_bar = scr->air_bar;

    lv_bar_set_value(o2_bar, o2_value, LV_ANIM_OFF);
    lv_bar_set_value(air_bar, air_value, LV_ANIM_OFF);

    lv_arc_set_value(scr->arc_1, arc1_value);
    lv_arc_set_value(scr->arc_2, arc2_value);

    if (o2_direct == 0) {
        o2_value++;
        if (o2_value > 100) {
            o2_value = 100;
            o2_direct = 1;
        }
    } else {
        o2_value--;
        if (o2_value < 0) {
            o2_value = 0;
            o2_direct = 0;
        }
    }

    if (air_direct == 0) {
        air_value++;
        if (air_value > 100) {
            air_value = 100;
            air_direct = 1;
        }
    } else {
        air_value--;
        if (air_value < 0) {
            air_value = 0;
            air_direct = 0;
        }
    }

    if (arc1_direct == 0) {
        arc1_value++;
        if (arc1_value > 100) {
            arc1_value = 100;
            arc1_direct = 1;
        }
    } else {
        arc1_value--;
        if (arc1_value < 0) {
            arc1_value = 0;
            arc1_direct = 0;
        }
    }

    if (arc2_direct == 0) {
        arc2_value++;
        if (arc2_value > 100) {
            arc2_value = 100;
            arc2_direct = 1;
        }
    } else {
        arc2_value--;
        if (arc2_value < 0) {
            arc2_value = 0;
            arc2_direct = 0;
        }
    }
}



static void num_timer_cb(lv_timer_t *timer) {
    int fio2_value = lv_bar_get_value(scr->o2_bar);
    int peepe_value = lv_bar_get_value(scr->air_bar);
    int ecg_value = (fio2_value + peepe_value) / 2;
    int vt1_value = lv_arc_get_value(scr->arc_1);
    int vt2_value = lv_arc_get_value(scr->arc_2);
    lv_label_set_text_fmt(scr->fio2_num, "%d", fio2_value);
    lv_label_set_text_fmt(scr->peepe_num, "%d", peepe_value);
    lv_label_set_text_fmt(scr->ecg_num, "%d", ecg_value);
    lv_label_set_text_fmt(scr->vt_1, "%d", vt1_value);
    lv_label_set_text_fmt(scr->vt_2, "%d", vt2_value * 5);

}

