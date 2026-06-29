#include "lv_obj_drag.h"
#include "aic_ui.h"

#if LV_USE_OBJ_DRAG && LV_BUILD_EXAMPLES

const char *drag1_months[] = {"January", "February", "March", "April", "May", "June", "July", \
                                "August", "September", "October", "November", "December"};


lv_obj_t *drag1_lv_create_container(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_outline_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(obj, 100, 160);
    lv_obj_set_pos(obj, 0, 0);

    return obj;
}

static void drag1_event_handler(lv_event_t * e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        int index = lv_obj_drag_get_active_id(target);
        LV_LOG_USER("Selected month: %s\n", drag1_months[index]);
    }
}

/**
 * An infinite vertical drag with the name of months.
 */
void lv_example_drag_1(void)
{
    lv_obj_t *container = drag1_lv_create_container(lv_screen_active());
    lv_obj_set_size(container, 100, 160);
    lv_obj_set_pos(container, 0, 0);
    lv_obj_t *drag1 = lv_obj_drag_create(container);
    lv_obj_drag_add_label_item(drag1, drag1_months, sizeof(drag1_months) / sizeof(drag1_months[0]), 100, 32, &lv_font_montserrat_16, lv_color_hex(0x000000), 16);
    lv_obj_drag_set_infinite(drag1, true);
    lv_obj_add_event_cb(drag1, drag1_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_drag_set_active(drag1, 0, LV_ANIM_OFF);
}

#endif
