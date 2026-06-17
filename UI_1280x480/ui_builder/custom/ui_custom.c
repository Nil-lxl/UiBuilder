#include "custom.h"
#include "ui_objects.h"

static scrMain_t *scrMain;
static scrCopy_t *scrCopy;
static scrScan_t *scrScan;
static scrPrint_t *scrPrint;
static scrSetup_t *scrSetup;
static scrLoad_t *scrLoad;
static scrPrintFinish_t *scrPrintFinish;

static int bright_value = 0;
static int hue_value = 0;

static copy_num = 1;

static lv_obj_t *img;

/* scr main*/
static lv_style_t bar1_indicator_style;
static lv_style_t bar2_indicator_style;
static lv_style_t bar4_indicator_style;
static lv_style_t bar3_indicator_style;

void scrMain_custom_load_start() {
    scrMain = scrMain_get(&ui_manager);
    lv_obj_t *bar1 = scrMain->bar_1;
    lv_obj_t *bar2 = scrMain->bar_2;
    lv_obj_t *bar3 = scrMain->bar_3;
    lv_obj_t *bar4 = scrMain->bar_4;

    lv_style_set_bg_color(&bar1_indicator_style, lv_color_hex(0x2ad3ff));
    lv_style_set_bg_color(&bar2_indicator_style, lv_color_hex(0xef1382));
    lv_style_set_bg_color(&bar3_indicator_style, lv_color_hex(0xe4ea09));
    lv_style_set_bg_color(&bar4_indicator_style, lv_color_hex(0x464eb1));

    lv_obj_add_style(bar1, &bar1_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(bar2, &bar2_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(bar3, &bar3_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(bar4, &bar4_indicator_style, LV_PART_INDICATOR);

    lv_anim_t anim_bar1;
    lv_anim_init(&anim_bar1);
    lv_anim_set_exec_cb(&anim_bar1, lv_bar_set_value);
    lv_anim_set_values(&anim_bar1, 20, 80);
    lv_anim_set_duration(&anim_bar1, 2000);
    lv_anim_set_delay(&anim_bar1, 100);
    lv_anim_set_var(&anim_bar1, bar1);
    lv_anim_set_playback_duration(&anim_bar1, 1000);
    lv_anim_set_repeat_count(&anim_bar1, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_bar1);

    lv_anim_t anim_bar2;
    lv_anim_init(&anim_bar2);
    lv_anim_set_exec_cb(&anim_bar2, lv_bar_set_value);
    lv_anim_set_values(&anim_bar2, 70, 10);
    lv_anim_set_duration(&anim_bar2, 1000);
    lv_anim_set_delay(&anim_bar2, 100);
    lv_anim_set_var(&anim_bar2, bar2);
    lv_anim_set_playback_duration(&anim_bar2, 1000);
    lv_anim_set_repeat_count(&anim_bar2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_bar2);

    lv_anim_t anim_bar3;
    lv_anim_init(&anim_bar3);
    lv_anim_set_exec_cb(&anim_bar3, lv_bar_set_value);
    lv_anim_set_values(&anim_bar3, 40, 100);
    lv_anim_set_duration(&anim_bar3, 1500);
    lv_anim_set_delay(&anim_bar3, 100);
    lv_anim_set_var(&anim_bar3, bar3);
    lv_anim_set_playback_duration(&anim_bar3, 1000);
    lv_anim_set_repeat_count(&anim_bar3, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_bar3);

    lv_anim_t anim_bar4;
    lv_anim_init(&anim_bar4);
    lv_anim_set_exec_cb(&anim_bar4, lv_bar_set_value);
    lv_anim_set_values(&anim_bar4, 0, 60);
    lv_anim_set_duration(&anim_bar4, 1000);
    lv_anim_set_delay(&anim_bar4, 100);
    lv_anim_set_var(&anim_bar4, bar4);
    lv_anim_set_playback_duration(&anim_bar4, 1000);
    lv_anim_set_repeat_count(&anim_bar4, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_bar4);


}

/* scr scan */
static lv_style_t slider_indicator_style;
static lv_style_t slider_knob_style;

void scrScan_custom_load_start() {
    scrScan = scrScan_get(&ui_manager);

    lv_obj_t *bright_slider = scrScan->slider_bright;
    lv_obj_t *hue_slider = scrScan->slider_hue;

    lv_style_init(&slider_indicator_style);
    lv_style_set_bg_color(&slider_indicator_style, lv_color_hex(0x3f65b3));

    lv_style_init(&slider_knob_style);
    lv_style_set_bg_color(&slider_knob_style, lv_color_hex(0x2f3243));
    lv_style_set_height(&slider_knob_style, 50);
    lv_style_set_radius(&slider_knob_style, 5);

    lv_obj_add_style(bright_slider, &slider_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(bright_slider, &slider_knob_style, LV_PART_KNOB);

    lv_obj_add_style(hue_slider, &slider_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(hue_slider, &slider_knob_style, LV_PART_KNOB);

}


void scrScan_slider_hue_custom_value_changed() {
    img = scrScan->image_bird;
    bright_value = lv_slider_get_value(scrScan->slider_bright);
    hue_value = lv_slider_get_value(scrScan->slider_hue);

    lv_color_t recolor;
    recolor = lv_color_hsv_to_rgb(hue_value, 100, bright_value);
    lv_obj_set_style_image_recolor(img, recolor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(img, 50, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void scrScan_slider_bright_custom_value_changed() {
    img = scrScan->image_bird;
    bright_value = lv_slider_get_value(scrScan->slider_bright);
    hue_value = lv_slider_get_value(scrScan->slider_hue);

    lv_color_t recolor;
    recolor = lv_color_hsv_to_rgb(hue_value, 100, bright_value);
    lv_obj_set_style_image_recolor(img, recolor, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(img, 50, LV_PART_MAIN | LV_STATE_DEFAULT);

}

/* scr copy */

void scrCopy_custom_load_start() {
    scrCopy = scrCopy_get(&ui_manager);
}

void scrCopy_btn_minus_custom_clicked() {
    if (copy_num <= 0) {
        return;
    }
    copy_num--;
    lv_label_set_text_fmt(scrCopy->label_num, "%d", copy_num);
}

void scrCopy_btn_plus_custom_clicked() {
    if (copy_num >= 10) {
        return;
    }
    copy_num++;
    lv_label_set_text_fmt(scrCopy->label_num, "%d", copy_num);
}

/* scr loader */

static lv_style_t arc_style;
static lv_style_t arc_bg_style;

static void set_label_value(void *obj, int32_t v) {
    lv_label_set_text_fmt(obj, "%d%%", (char *)v);
}

static void anim_completed_cb(lv_anim_t *anim) {
    lv_obj_t *act_scr = lv_scr_act();
    if (!screen_is_loading(act_scr)) {
        // delete child obj of act_scr
        lv_obj_clean(act_scr);
        // create scrMain
        scrPrintFinish_create(&ui_manager);
        // load scrMain
        scrPrintFinish = scrPrintFinish_get(&ui_manager);
        lv_scr_load_anim(scrPrintFinish->obj, LV_SCR_LOAD_ANIM_FADE_OUT, 500, 0, ui_manager.auto_del);
    }
}
void scrLoad_custom_loaded() {
    scrLoad = scrLoad_get(&ui_manager);

    lv_obj_t *arc = scrLoad->arc_prog;
    lv_obj_t *label = scrLoad->label_prog;

    lv_style_init(&arc_style);
    lv_style_set_arc_rounded(&arc_style, 20);
    lv_style_set_arc_width(&arc_style, 20);
    lv_style_set_arc_color(&arc_style, lv_color_hex(0xffffff));

    lv_style_init(&arc_bg_style);
    lv_style_set_arc_rounded(&arc_bg_style, 0);
    lv_style_set_arc_width(&arc_bg_style, 20);
    lv_style_set_arc_opa(&arc_bg_style, LV_OPA_0);
    lv_style_set_arc_color(&arc_bg_style, lv_palette_main(LV_PALETTE_CYAN));

    lv_obj_add_style(arc, &arc_bg_style, LV_PART_MAIN);
    lv_obj_add_style(arc, &arc_style, LV_PART_INDICATOR);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);

    lv_anim_t loader_arc;
    lv_anim_init(&loader_arc);
    lv_anim_set_exec_cb(&loader_arc, (lv_anim_exec_xcb_t)lv_arc_set_value);
    lv_anim_set_ready_cb(&loader_arc, anim_completed_cb);
    lv_anim_set_values(&loader_arc, 0, 100);
    lv_anim_set_duration(&loader_arc, 2000);
    lv_anim_set_delay(&loader_arc, 100);
    lv_anim_set_var(&loader_arc, arc);
    lv_anim_start(&loader_arc);

    lv_anim_t loader_label;
    lv_anim_init(&loader_label);
    lv_anim_set_exec_cb(&loader_label, set_label_value);
    lv_anim_set_values(&loader_label, 0, 100);
    lv_anim_set_duration(&loader_label, 2000);
    lv_anim_set_delay(&loader_label, 100);
    lv_anim_set_var(&loader_label, label);
    lv_anim_start(&loader_label);


}
