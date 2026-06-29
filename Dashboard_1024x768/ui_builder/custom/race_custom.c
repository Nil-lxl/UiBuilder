#include "custom.h"
#include "ui_objects.h"

static scrRace_t *scrRace;

lv_anim_t anim_gradient_left;
lv_anim_t anim_gradient_right;
lv_anim_t anim_label_speed;
lv_anim_t anim_label_rpm;

static lv_timer_t *dir_timer;
static lv_timer_t *toggle_scr_timer;

static int count = 0;

#define RACE_ANIM_DURATION      3000

static void toggle_timer_cb(lv_timer_t *timer) {
    lv_obj_t *act_scr = lv_scr_act();
    if (!screen_is_loading(act_scr)) {
        // delete child obj of act_scr
        lv_obj_clean(act_scr);
        // create scrAbout
        scrSport_create(&ui_manager);
        // load scrAbout
        lv_scr_load_anim(scrSport_get(&ui_manager)->obj, LV_SCR_LOAD_ANIM_NONE, 0, 0, ui_manager.auto_del);
    }
}

static void dir_timer_cb(lv_timer_t *timer) {
    switch (count % 3) {
    case 0:
        lv_label_set_text(scrRace->label_dist, "420m");
        lv_label_set_text(scrRace->label_dir, "右转进入中山路");
        lv_img_set_src(scrRace->img_dir, LVGL_IMAGE_PATH(turn/right.png));
        lv_obj_set_style_img_recolor(scrRace->img_light, lv_color_hex(0x00aa00), LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    case 1:
        lv_label_set_text(scrRace->label_dist, "370m");
        lv_label_set_text(scrRace->label_dir, "左转进入澳门大道");
        lv_img_set_src(scrRace->img_dir, LVGL_IMAGE_PATH(turn/left.png));
        lv_obj_set_style_img_recolor(scrRace->img_light, lv_color_hex(0xaa0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    case 2:
        lv_label_set_text(scrRace->label_dist, "500m");
        lv_label_set_text(scrRace->label_dir, "直行进入珠海大桥");
        lv_img_set_src(scrRace->img_dir, LVGL_IMAGE_PATH(turn/straight2.png));
        lv_obj_set_style_img_recolor(scrRace->img_light, lv_color_hex(0xffaa00), LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    default:
        break;
    }
    count++;
}

static void anim_gradient_cb(void *obj, int value) {
    lv_obj_set_y(obj, value);
}

static void anim_label_spped_cb(void *obj, int value) {
    lv_label_set_text_fmt(obj, "%d", value);
}

static void anim_label_rpm_cb(void *obj, int value) {
    lv_label_set_text_fmt(obj, "%d.%d", value / 10, value % 10);

}

void scrRace_custom_load_start() {
    scrRace = scrRace_get(&ui_manager);

    lv_anim_init(&anim_gradient_left);
    lv_anim_set_var(&anim_gradient_left, scrRace->cont_cover1);
    lv_anim_set_exec_cb(&anim_gradient_left, anim_gradient_cb);
    lv_anim_set_values(&anim_gradient_left, 295, 130);
    lv_anim_set_duration(&anim_gradient_left, RACE_ANIM_DURATION);
    lv_anim_set_path_cb(&anim_gradient_left, lv_anim_path_ease_in_out);
    lv_anim_set_playback_duration(&anim_gradient_left, RACE_ANIM_DURATION);
    lv_anim_set_repeat_delay(&anim_gradient_left, 0);
    lv_anim_set_repeat_count(&anim_gradient_left, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_gradient_left);

    lv_anim_init(&anim_gradient_right);
    lv_anim_set_var(&anim_gradient_right, scrRace->cont_cover2);
    lv_anim_set_exec_cb(&anim_gradient_right, anim_gradient_cb);
    lv_anim_set_values(&anim_gradient_right, 295, 130);
    lv_anim_set_duration(&anim_gradient_right, RACE_ANIM_DURATION);
    lv_anim_set_path_cb(&anim_gradient_right, lv_anim_path_ease_in_out);
    lv_anim_set_playback_duration(&anim_gradient_right, RACE_ANIM_DURATION);
    lv_anim_set_repeat_delay(&anim_gradient_right, 0);
    lv_anim_set_repeat_count(&anim_gradient_right, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_gradient_right);

    lv_anim_init(&anim_label_speed);
    lv_anim_set_var(&anim_label_speed, scrRace->label_speed);
    lv_anim_set_exec_cb(&anim_label_speed, anim_label_spped_cb);
    lv_anim_set_values(&anim_label_speed, 0, 200);
    lv_anim_set_duration(&anim_label_speed, RACE_ANIM_DURATION);
    lv_anim_set_path_cb(&anim_label_speed, lv_anim_path_ease_in_out);
    lv_anim_set_playback_duration(&anim_label_speed, RACE_ANIM_DURATION);
    lv_anim_set_repeat_delay(&anim_label_speed, 0);
    lv_anim_set_repeat_count(&anim_label_speed, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_speed);

    lv_anim_init(&anim_label_rpm);
    lv_anim_set_var(&anim_label_rpm, scrRace->label_rpm);
    lv_anim_set_exec_cb(&anim_label_rpm, anim_label_rpm_cb);
    lv_anim_set_values(&anim_label_rpm, 0, 99);
    lv_anim_set_duration(&anim_label_rpm, RACE_ANIM_DURATION);
    lv_anim_set_path_cb(&anim_label_rpm, lv_anim_path_ease_in_out);
    lv_anim_set_playback_duration(&anim_label_rpm, RACE_ANIM_DURATION);
    lv_anim_set_repeat_delay(&anim_label_rpm, 0);
    lv_anim_set_repeat_count(&anim_label_rpm, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim_label_rpm);

    dir_timer = lv_timer_create(dir_timer_cb, 3000, NULL);
    toggle_scr_timer = lv_timer_create(toggle_timer_cb, 10000, NULL);

}

void scrRace_custom_unloaded() {
    lv_timer_delete(dir_timer);
    lv_timer_delete(toggle_scr_timer);
}