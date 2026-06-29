#include "custom.h"
#include "ui_objects.h"

static scrMain_t *scrMain;

lv_anim_t anim_arc_red;
lv_anim_t anim_arc_green;
lv_anim_t anim_arc_purple;

lv_anim_t anim_label_calories;
lv_anim_t anim_label_mainsteps;
lv_anim_t anim_label_steps;
lv_anim_t anim_label_min;
lv_anim_t anim_label_bpm;

lv_anim_t anim_arc_spo2;
lv_anim_t anim_arc_brpm;
lv_anim_t anim_arc_sleep;

lv_anim_t anim_label_spo2;
lv_anim_t anim_label_brpm;
lv_anim_t anim_label_slp_min;

static void anim_arc_cb(void *var, int value) {
    lv_arc_set_value(var, value);
}

static void set_label_percent(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "%d%%", v);
}
static void set_label_num(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "%d", v);
}

void screen_custom_load_start() {
    scrMain = scrMain_get(&ui_manager);

    lv_obj_set_style_width(scrMain->chart_calory, 0, LV_PART_INDICATOR);
    lv_obj_set_style_height(scrMain->chart_calory, 0, LV_PART_INDICATOR);

    // lv_obj_set_style_width(scrMain->chart_bpm, 0, LV_PART_INDICATOR);
    // lv_obj_set_style_height(scrMain->chart_bpm, 0, LV_PART_INDICATOR);


    lv_anim_init(&anim_arc_red);
    lv_anim_set_var(&anim_arc_red, scrMain->arc_red);
    lv_anim_set_exec_cb(&anim_arc_red, anim_arc_cb);
    lv_anim_set_values(&anim_arc_red, 0, 600);
    lv_anim_set_duration(&anim_arc_red, 6000);
    lv_anim_set_playback_duration(&anim_arc_red, 2000);
    lv_anim_set_repeat_count(&anim_arc_red, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_arc_red);

    lv_anim_init(&anim_arc_green);
    lv_anim_set_var(&anim_arc_green, scrMain->arc_green);
    lv_anim_set_exec_cb(&anim_arc_green, anim_arc_cb);
    lv_anim_set_values(&anim_arc_green, 0, 1000);
    lv_anim_set_duration(&anim_arc_green, 3000);
    lv_anim_set_playback_duration(&anim_arc_green, 2000);
    lv_anim_set_repeat_count(&anim_arc_green, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_arc_green);

    lv_anim_init(&anim_arc_purple);
    lv_anim_set_var(&anim_arc_purple, scrMain->arc_purple);
    lv_anim_set_exec_cb(&anim_arc_purple, anim_arc_cb);
    lv_anim_set_values(&anim_arc_purple, 0, 200);
    lv_anim_set_duration(&anim_arc_purple, 12000);
    lv_anim_set_playback_duration(&anim_arc_purple, 2000);
    lv_anim_set_repeat_count(&anim_arc_purple, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_arc_purple);

    lv_anim_init(&anim_label_calories);
    lv_anim_set_var(&anim_label_calories, scrMain->label_calories);
    lv_anim_set_exec_cb(&anim_label_calories, set_label_num);
    lv_anim_set_values(&anim_label_calories, 0, 650);
    lv_anim_set_duration(&anim_label_calories, 30000);
    // lv_anim_set_playback_duration(&anim_label_calories, 10000);
    lv_anim_set_repeat_delay(&anim_label_calories, 1000);
    lv_anim_set_repeat_count(&anim_label_calories, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_calories);

    lv_anim_init(&anim_label_steps);
    lv_anim_set_var(&anim_label_steps, scrMain->label_steps);
    lv_anim_set_exec_cb(&anim_label_steps, set_label_num);
    lv_anim_set_values(&anim_label_steps, 8200, 8526);
    lv_anim_set_duration(&anim_label_steps, 60000);
    // lv_anim_set_playback_duration(&anim_label_calories, 10000);
    lv_anim_set_repeat_delay(&anim_label_steps, 1000);
    lv_anim_set_repeat_count(&anim_label_steps, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_steps);

    lv_anim_init(&anim_label_mainsteps);
    lv_anim_set_var(&anim_label_mainsteps, scrMain->label_mainstep);
    lv_anim_set_exec_cb(&anim_label_mainsteps, set_label_num);
    lv_anim_set_values(&anim_label_mainsteps, 8200, 8526);
    lv_anim_set_duration(&anim_label_mainsteps, 60000);
    // lv_anim_set_playback_duration(&anim_label_calories, 10000);
    lv_anim_set_repeat_delay(&anim_label_mainsteps, 1000);
    lv_anim_set_repeat_count(&anim_label_mainsteps, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_mainsteps);

    lv_anim_init(&anim_label_min);
    lv_anim_set_var(&anim_label_min, scrMain->label_min);
    lv_anim_set_exec_cb(&anim_label_min, set_label_num);
    lv_anim_set_values(&anim_label_min, 0, 60);
    lv_anim_set_duration(&anim_label_min, 60000);
    // lv_anim_set_playback_duration(&anim_label_calories, 10000);
    lv_anim_set_repeat_delay(&anim_label_min, 1000);
    lv_anim_set_repeat_count(&anim_label_min, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_min);

    lv_anim_init(&anim_label_bpm);
    lv_anim_set_var(&anim_label_bpm, scrMain->label_bpm);
    lv_anim_set_exec_cb(&anim_label_bpm, set_label_num);
    lv_anim_set_values(&anim_label_bpm, 56, 128);
    lv_anim_set_duration(&anim_label_bpm, 10000);
    // lv_anim_set_playback_duration(&anim_label_calories, 10000);
    lv_anim_set_repeat_delay(&anim_label_bpm, 1000);
    lv_anim_set_repeat_count(&anim_label_bpm, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_bpm);

    lv_anim_init(&anim_arc_spo2);
    lv_anim_set_var(&anim_arc_spo2, scrMain->arc_spo2);
    lv_anim_set_exec_cb(&anim_arc_spo2, anim_arc_cb);
    lv_anim_set_values(&anim_arc_spo2, 20, 200);
    lv_anim_set_duration(&anim_arc_spo2, 5000);
    lv_anim_set_playback_duration(&anim_arc_spo2, 2000);
    lv_anim_set_repeat_count(&anim_arc_spo2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_arc_spo2);

    lv_anim_init(&anim_arc_brpm);
    lv_anim_set_var(&anim_arc_brpm, scrMain->arc_brpm);
    lv_anim_set_exec_cb(&anim_arc_brpm, anim_arc_cb);
    lv_anim_set_values(&anim_arc_brpm, 10, 200);
    lv_anim_set_duration(&anim_arc_brpm, 4000);
    lv_anim_set_playback_duration(&anim_arc_brpm, 2000);
    lv_anim_set_repeat_count(&anim_arc_brpm, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_arc_brpm);

    lv_anim_init(&anim_arc_sleep);
    lv_anim_set_var(&anim_arc_sleep, scrMain->arc_sleep);
    lv_anim_set_exec_cb(&anim_arc_sleep, anim_arc_cb);
    lv_anim_set_values(&anim_arc_sleep, 30, 200);
    lv_anim_set_duration(&anim_arc_sleep, 10000);
    lv_anim_set_playback_duration(&anim_arc_sleep, 2000);
    lv_anim_set_repeat_count(&anim_arc_sleep, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_arc_sleep);


    lv_anim_init(&anim_label_spo2);
    lv_anim_set_var(&anim_label_spo2, scrMain->label_spo2);
    lv_anim_set_exec_cb(&anim_label_spo2, set_label_percent);
    lv_anim_set_values(&anim_label_spo2, 20, 100);
    lv_anim_set_duration(&anim_label_spo2, 5000);
    lv_anim_set_playback_duration(&anim_label_spo2, 2000);
    // lv_anim_set_repeat_delay(&anim_label_spo2, 1000);
    lv_anim_set_repeat_count(&anim_label_spo2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_spo2);

    lv_anim_init(&anim_label_brpm);
    lv_anim_set_var(&anim_label_brpm, scrMain->label_brpm);
    lv_anim_set_exec_cb(&anim_label_brpm, set_label_num);
    lv_anim_set_values(&anim_label_brpm, 10, 50);
    lv_anim_set_duration(&anim_label_brpm, 4000);
    lv_anim_set_playback_duration(&anim_label_brpm, 2000);
    // lv_anim_set_repeat_delay(&anim_label_brpm, 1000);
    lv_anim_set_repeat_count(&anim_label_brpm, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_brpm);

    lv_anim_init(&anim_label_slp_min);
    lv_anim_set_var(&anim_label_slp_min, scrMain->label_slp_min);
    lv_anim_set_exec_cb(&anim_label_slp_min, set_label_num);
    lv_anim_set_values(&anim_label_slp_min, 30, 60);
    lv_anim_set_duration(&anim_label_slp_min, 10000);
    lv_anim_set_playback_duration(&anim_label_slp_min, 2000);
    // lv_anim_set_repeat_delay(&anim_label_slp_min, 1000);
    lv_anim_set_repeat_count(&anim_label_slp_min, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_slp_min);

}