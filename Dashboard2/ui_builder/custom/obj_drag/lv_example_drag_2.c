#include "lv_obj_drag.h"
#include "aic_ui.h"

#if LV_USE_OBJ_DRAG && LV_BUILD_EXAMPLES

const char *drag2_img_list[] = {"bread1", "bread2", "bread3", "bread4", "bread5", "bread6"};

const char *drag2_img_path[] = {
    LVGL_IMAGE_PATH(bread/bread1.png),
    LVGL_IMAGE_PATH(bread/bread2.png),
    LVGL_IMAGE_PATH(bread/bread3.png),
    LVGL_IMAGE_PATH(bread/bread4.png),
    LVGL_IMAGE_PATH(bread/bread5.png),
    LVGL_IMAGE_PATH(bread/bread6.png) };

lv_obj_t *drag2_lv_create_container(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_outline_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(obj, 1000, 200);
    lv_obj_set_pos(obj, 0, 0);

    return obj;
}

static void drag2_event_handler(lv_event_t * e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        int index = lv_obj_drag_get_active_id(target);
        LV_LOG_USER("Selected bread: %s\n", drag2_img_list[index]);
    }
}

void lv_example_drag_2(void)
{
    lv_obj_t *container = drag2_lv_create_container(lv_screen_active());
    lv_obj_set_size(container, 1000, 200);
    lv_obj_set_pos(container, 0, 0);
    lv_obj_t *drag2 = lv_obj_drag_create(container);
    lv_obj_drag_set_dir(drag2, LV_DRAG_DIR_HOR);
    lv_obj_drag_add_img_item(drag2, drag2_img_path, 6, 200, 200);
    lv_obj_drag_set_infinite(drag2, true);
    lv_obj_drag_enable_opa_change(drag2, true, true);
    lv_obj_drag_set_opa_change_mode(drag2, LV_DRAG_SOLID);
    lv_obj_drag_set_opa_ratio(drag2, 255, 100, 100);
    lv_obj_drag_enable_zoom_change(drag2, true, true);
    lv_obj_drag_set_zoom_change_mode(drag2, LV_DRAG_GRADIENT, LV_DRAG_ZOOM_CENTER);
    lv_obj_drag_set_zoom_ratio(drag2, 256, 100, 400);
    lv_obj_add_event_cb(drag2, drag2_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_drag_set_active(drag2, 0, LV_ANIM_OFF);
}

#endif
