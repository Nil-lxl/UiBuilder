#include "custom.h"
#include "ui_objects.h"

static scrMain_t *scrMain;
static scrCharge_t *scrCharge;
static scrSmartHome_t *scrSmartHome;


void scrMain_custom_load_start() {

    scrMain = scrMain_get(&ui_manager);

    lv_obj_t *cont_item = scrMain->cont_item;
    lv_obj_set_flex_flow(cont_item, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont_item, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_SPACE_BETWEEN);

    lv_obj_set_style_margin_hor(scrMain->contItem1, 50, 0);
    lv_obj_set_style_margin_hor(scrMain->contItem2, 50, 0);
    lv_obj_set_style_margin_hor(scrMain->contItem3, 50, 0);
    lv_obj_set_style_margin_hor(scrMain->contItem4, 50, 0);
    lv_obj_set_style_margin_hor(scrMain->contItem5, 50, 0);

}

/* scr Charging */
static lv_style_t arc_charge_style;

lv_anim_timeline_t *anim_timeline_charge;
lv_anim_t anim_charge;
lv_anim_t anim_label_perc;
lv_anim_t anim_label_time;
lv_anim_t anim_label_consume;
lv_anim_t anim_label_km;
lv_anim_t anim_label_money;

static void anim_arc_cb(void *var, int value) {
    lv_arc_set_value(var, value);
}
static void set_label_percent(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "%d%%", v);
}
static void set_label_time(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "%d.%d", v / 10, v % 10);
}
static void set_label_consume(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "%d.%d%d", v / 100, v % 100 / 10, v % 10);
}
static void set_label_km(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "%d", v);
}
static void set_label_money(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "$%d", v);
}

bool isCharging = false;

void scrCharge_btn_charge_custom_clicked() {
    lv_obj_t *charge_btn_label = lv_obj_get_child(scrCharge->btn_charge, 0);

    if (isCharging) {
        lv_anim_timeline_pause(anim_timeline_charge);
        isCharging = false;
        lv_obj_set_style_bg_color(scrCharge->cont_charge, lv_color_hex(0x9364f0), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(scrCharge->cont_charge, lv_color_hex(0xd177be), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(charge_btn_label, "%s", "Start Charging");
    } else {
        lv_anim_timeline_start(anim_timeline_charge);
        isCharging = true;
        lv_obj_set_style_bg_color(scrCharge->cont_charge, lv_color_hex(0x759132), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(scrCharge->cont_charge, lv_color_hex(0x21d684), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(charge_btn_label, "%s", "Stop Charging");

    }

}

void scrCharge_custom_load_start() {
    scrCharge = scrCharge_get(&ui_manager);

    lv_style_init(&arc_charge_style);
    lv_style_set_arc_width(&arc_charge_style, 20);
    lv_style_set_arc_color(&arc_charge_style, lv_color_hex(0xffffff));
    lv_style_set_radius(&arc_charge_style, 5);

    lv_obj_t *arc_charge = scrCharge->arc_charge;
    lv_obj_add_style(arc_charge, &arc_charge_style, LV_PART_INDICATOR);
    lv_obj_remove_style(arc_charge, NULL, LV_PART_KNOB);

    /* Animations */

    lv_anim_init(&anim_charge);
    lv_anim_set_var(&anim_charge, arc_charge);
    lv_anim_set_exec_cb(&anim_charge, anim_arc_cb);
    lv_anim_set_values(&anim_charge, 0, 100);
    lv_anim_set_duration(&anim_charge, 40000);
    // lv_anim_set_repeat_delay(&anim_charge, 1000);

    lv_anim_init(&anim_label_perc);
    lv_anim_set_var(&anim_label_perc, scrCharge->label_percent);
    lv_anim_set_exec_cb(&anim_label_perc, set_label_percent);
    lv_anim_set_values(&anim_label_perc, 0, 100);
    lv_anim_set_duration(&anim_label_perc, 40000);
    // lv_anim_set_delay(&anim_label_perc, 100);

    lv_anim_init(&anim_label_time);
    lv_anim_set_var(&anim_label_time, scrCharge->label_time);
    lv_anim_set_exec_cb(&anim_label_time, set_label_time);
    lv_anim_set_values(&anim_label_time, 72, 0);
    lv_anim_set_duration(&anim_label_time, 40000);

    lv_anim_init(&anim_label_consume);
    lv_anim_set_var(&anim_label_consume, scrCharge->label_consume);
    lv_anim_set_exec_cb(&anim_label_consume, set_label_consume);
    lv_anim_set_values(&anim_label_consume, 0, 1000);
    lv_anim_set_duration(&anim_label_consume, 40000);

    lv_anim_init(&anim_label_km);
    lv_anim_set_var(&anim_label_km, scrCharge->label_km);
    lv_anim_set_exec_cb(&anim_label_km, set_label_km);
    lv_anim_set_values(&anim_label_km, 1, 300);
    lv_anim_set_duration(&anim_label_km, 40000);

    lv_anim_init(&anim_label_money);
    lv_anim_set_var(&anim_label_money, scrCharge->label_money);
    lv_anim_set_exec_cb(&anim_label_money, set_label_money);
    lv_anim_set_values(&anim_label_money, 190, 300);
    lv_anim_set_duration(&anim_label_money, 40000);

    anim_timeline_charge = lv_anim_timeline_create();
    lv_anim_timeline_add(anim_timeline_charge, 0, &anim_charge);
    lv_anim_timeline_add(anim_timeline_charge, 0, &anim_label_perc);
    lv_anim_timeline_add(anim_timeline_charge, 0, &anim_label_time);
    lv_anim_timeline_add(anim_timeline_charge, 0, &anim_label_consume);
    lv_anim_timeline_add(anim_timeline_charge, 0, &anim_label_km);
    lv_anim_timeline_add(anim_timeline_charge, 0, &anim_label_money);

    lv_anim_timeline_set_progress(anim_timeline_charge, 0);

}

void scrCharge_arc_charge_custom_value_changed() {
    int value = lv_arc_get_value(scrCharge->arc_charge);
    lv_label_set_text_fmt(scrCharge->label_percent, "%d%%", value);
}

void scrCharge_custom_unloaded() {
    lv_anim_timeline_delete(anim_timeline_charge);
    isCharging = false;
}

/* scr SmartHome */
static lv_style_t lock_knob_style;
static lv_style_t lock_indicator_style;

static lv_style_t slider_light_knob_style;
static lv_style_t slider_light1_indicator_sytle;
static lv_style_t slider_light2_indicator_sytle;

lv_anim_t anim_smarthome_arc;
lv_anim_t anim_label_charge;

void scrSmartHome_custom_load_start() {
    scrSmartHome = scrSmartHome_get(&ui_manager);

    lv_style_init(&lock_knob_style);
    lv_style_init(&lock_indicator_style);

    lv_style_set_bg_image_src(&lock_knob_style, LVGL_IMAGE_PATH(lock.png));
    lv_style_set_bg_color(&lock_indicator_style, lv_color_hex(0x2dcc22));
    lv_style_set_bg_opa(&lock_indicator_style, LV_OPA_80);

    lv_style_init(&slider_light_knob_style);
    lv_style_init(&slider_light1_indicator_sytle);
    lv_style_init(&slider_light2_indicator_sytle);

    lv_style_set_size(&slider_light_knob_style, 10, 30);
    lv_style_set_max_height(&slider_light_knob_style, 10);
    lv_style_set_bg_color(&slider_light_knob_style, lv_color_hex(0xffffff));
    lv_style_set_radius(&slider_light_knob_style, 10);

    lv_style_set_radius(&slider_light1_indicator_sytle, 10);
    lv_style_set_bg_color(&slider_light1_indicator_sytle, lv_color_hex(0xffffff));
    lv_style_set_bg_opa(&slider_light1_indicator_sytle, LV_OPA_50);

    lv_style_set_radius(&slider_light2_indicator_sytle, 10);
    lv_style_set_bg_color(&slider_light2_indicator_sytle, lv_color_hex(0xffffff));
    lv_style_set_bg_opa(&slider_light2_indicator_sytle, LV_OPA_50);

    lv_obj_t *cont_main = scrSmartHome->cont_main;
    lv_obj_set_flex_flow(cont_main, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont_main, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_SPACE_BETWEEN);

    lv_obj_set_style_margin_hor(scrSmartHome->cont_1, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_2, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_3, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_4, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_5, 20, 0);

    lv_obj_add_style(scrSmartHome->switch_lock, &lock_knob_style, LV_PART_KNOB);
    lv_obj_add_style(scrSmartHome->switch_lock, &lock_indicator_style, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_t *slider_light1 = scrSmartHome->slider_light1;
    lv_obj_t *slider_light2 = scrSmartHome->slider_light2;

    lv_obj_add_style(slider_light1, &slider_light_knob_style, LV_PART_KNOB);
    lv_obj_add_style(slider_light2, &slider_light_knob_style, LV_PART_KNOB);

    lv_obj_add_style(slider_light1, &slider_light1_indicator_sytle, LV_PART_INDICATOR | LV_PART_MAIN);
    lv_obj_add_style(slider_light2, &slider_light2_indicator_sytle, LV_PART_INDICATOR | LV_PART_MAIN);

    lv_anim_init(&anim_smarthome_arc);
    lv_anim_set_var(&anim_smarthome_arc, scrSmartHome->arc_charge);
    lv_anim_set_exec_cb(&anim_smarthome_arc, anim_arc_cb);
    lv_anim_set_values(&anim_smarthome_arc, 0, 100);
    lv_anim_set_duration(&anim_smarthome_arc, 30000);
    lv_anim_start(&anim_smarthome_arc);
    // lv_anim_set_repeat_delay(&anim_charge, 1000);

    lv_anim_init(&anim_label_charge);
    lv_anim_set_var(&anim_label_charge, scrSmartHome->label_charge);
    lv_anim_set_exec_cb(&anim_label_charge, set_label_percent);
    lv_anim_set_values(&anim_label_charge, 0, 100);
    lv_anim_set_duration(&anim_label_charge, 30000);
    lv_anim_start(&anim_label_charge);
    // lv_anim_set_delay(&anim_label_perc, 100);
}