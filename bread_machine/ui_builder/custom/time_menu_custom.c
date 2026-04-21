#include "custom.h"
#include "obj_drag/lv_obj_drag.h"

static lv_obj_t *hour_roller = NULL;
static lv_obj_t *minute_roller = NULL;

const char *hour_time[] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", 
                            "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
const char *minute_time[] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                             "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                             "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
                             "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
                             "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
                             "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"};

void bread_time_menu_custom_created(void)
{
    bread_time_menu_t *scr = bread_time_menu_get(&ui_manager);

    hour_roller = lv_obj_drag_create(scr->hour_container);
    minute_roller = lv_obj_drag_create(scr->minute_container);
    for (int i = 0; i < 24; i++) {
        lv_obj_t *child = lv_label_create(hour_roller);

        lv_label_set_text(child, hour_time[i]);
        lv_label_set_long_mode(child, LV_LABEL_LONG_WRAP);
        lv_obj_set_size(child, 94, 56);
    
        lv_obj_set_style_text_font(child, aic_get_multi_lang_font(30), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(child, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(child, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align_to(child, hour_roller, LV_ALIGN_TOP_LEFT, 10, 0);
    }

    lv_obj_drag_set_infinite(hour_roller, true);

    lv_obj_drag_enable_opa_change(hour_roller, true, false);
    lv_obj_drag_set_opa_ratio(hour_roller, 255, 100, 112);

    lv_obj_drag_enable_zoom_change(hour_roller, true, false);
    lv_obj_drag_set_zoom_ratio(hour_roller, 256, 120, 112);

    lv_obj_drag_set_active(hour_roller, 0, LV_ANIM_OFF);

    minute_roller = lv_obj_drag_create(scr->minute_container);
    for (int i = 0; i < 60; i++) {
        lv_obj_t *child = lv_label_create(minute_roller);

        lv_label_set_text(child, minute_time[i]);
        lv_label_set_long_mode(child, LV_LABEL_LONG_WRAP);
        lv_obj_set_size(child, 94, 56);
    
        lv_obj_set_style_text_font(child, aic_get_multi_lang_font(30), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(child, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(child, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align_to(child, minute_roller, LV_ALIGN_TOP_LEFT, 10, 0);
    }

    lv_obj_drag_set_infinite(minute_roller, true);

    lv_obj_drag_enable_opa_change(minute_roller, true, false);
    lv_obj_drag_set_opa_ratio(minute_roller, 255, 100, 112);

    lv_obj_drag_enable_zoom_change(minute_roller, true, false);
    lv_obj_drag_set_zoom_ratio(minute_roller, 256, 120, 112);

    lv_obj_drag_set_active(minute_roller, 0, LV_ANIM_OFF);
}
