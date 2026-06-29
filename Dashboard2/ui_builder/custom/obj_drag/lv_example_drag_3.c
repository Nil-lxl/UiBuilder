#include "lv_obj_drag.h"
#include "aic_ui.h"

#if LV_USE_OBJ_DRAG && LV_BUILD_EXAMPLES

const char *drag3_img_list[] = {"bread1", "bread2", "bread3", "bread4", "bread5", "bread6"};

const char *drag3_img_path[] = {
    LVGL_IMAGE_PATH(bread/bread1.png),
    LVGL_IMAGE_PATH(bread/bread2.png),
    LVGL_IMAGE_PATH(bread/bread3.png),
    LVGL_IMAGE_PATH(bread/bread4.png),
    LVGL_IMAGE_PATH(bread/bread5.png),
    LVGL_IMAGE_PATH(bread/bread6.png) };

lv_obj_t *drag3_lv_create_container(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_outline_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

    return obj;
}

static void drag3_event_handler(lv_event_t * e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        int index = lv_obj_drag_get_active_id(target);
        LV_LOG_USER("Selected bread: %s\n", drag3_img_list[index]);
    }
}

void lv_example_drag_3(void)
{
    lv_obj_t *container = drag3_lv_create_container(lv_screen_active());
    lv_obj_set_size(container, 1000, 400);
    lv_obj_set_pos(container, 0, 0);
    lv_obj_t *drag3 = lv_obj_drag_create(container);
    lv_obj_drag_set_dir(drag3, LV_DRAG_DIR_HOR);
    for (int i = 0; i < sizeof(drag3_img_list) / sizeof(drag3_img_list[0]); i++) {
        lv_obj_t *bread_container = drag3_lv_create_container(drag3);
        lv_obj_set_size(bread_container, 200, 200);
        lv_obj_remove_flag(bread_container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(bread_container, LV_OBJ_FLAG_EVENT_BUBBLE);

        lv_obj_t *img_child = lv_img_create(bread_container);
        lv_obj_set_size(img_child, 200, 200);
        lv_img_set_src(img_child, drag3_img_path[i]);
        lv_img_set_pivot(img_child, 100, 100);
        lv_img_set_angle(img_child, 0);
        lv_obj_set_style_img_opa(img_child, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *label_child = lv_label_create(bread_container);
        lv_label_set_long_mode(label_child, LV_LABEL_LONG_WRAP);
        lv_label_set_text(label_child, drag3_img_list[i]);
        lv_obj_set_size(label_child, 100, 32);
        lv_obj_set_style_text_font(label_child, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label_child, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(label_child, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(label_child, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align_to(label_child, bread_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    lv_obj_drag_enable_transform_mode(drag3, true);
    lv_point_t point[3];
    point[0].x = 0;
    point[0].y = 0;
    point[1].x = 40;
    point[1].y = 65;
    point[2].x = 65;
    point[2].y = 130;
    lv_obj_drag_add_offset_point(drag3, point, 3);
    lv_obj_update_layout(drag3);
    lv_obj_drag_set_infinite(drag3, true);
    lv_obj_drag_enable_opa_change(drag3, true, false);
    lv_obj_drag_set_opa_change_mode(drag3, LV_DRAG_SOLID);
    lv_obj_drag_set_opa_ratio(drag3, 255, 100, 100);
    lv_obj_drag_enable_zoom_change(drag3, true, false);
    lv_obj_drag_set_zoom_change_mode(drag3, LV_DRAG_GRADIENT, LV_DRAG_ZOOM_CENTER);
    lv_obj_drag_set_zoom_ratio(drag3, 256, 100, 400);
    lv_obj_add_event_cb(drag3, drag3_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_drag_set_active(drag3, 0, LV_ANIM_OFF);
}

#endif
