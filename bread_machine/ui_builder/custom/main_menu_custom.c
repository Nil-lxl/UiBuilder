#include "custom.h"
#include "obj_drag/lv_obj_drag.h"

static lv_obj_t *bread_roller = NULL;

const char *bread_image[] = {
    LVGL_IMAGE_PATH(bread/bread1.png),
    LVGL_IMAGE_PATH(bread/bread2.png),
    LVGL_IMAGE_PATH(bread/bread3.png),
    LVGL_IMAGE_PATH(bread/bread4.png),
    LVGL_IMAGE_PATH(bread/bread5.png),
    LVGL_IMAGE_PATH(bread/bread6.png)};

void bread_main_menu_custom_created(void) 
{   
    bread_main_menu_t *scr = bread_main_menu_get(&ui_manager);

    bread_roller = lv_obj_drag_create(scr->bread_container);

    lv_obj_drag_add_img_item(bread_roller, bread_image, 6, 200, 200);
    lv_obj_drag_set_dir(bread_roller, LV_DRAG_DIR_HOR);
    lv_obj_drag_set_infinite(bread_roller, true);

    lv_obj_drag_enable_opa_change(bread_roller, true, true);
    lv_obj_drag_set_opa_ratio(bread_roller, 255, 100, 400);

    lv_obj_drag_enable_zoom_change(bread_roller, true, true);
    lv_obj_drag_set_zoom_ratio(bread_roller, 256, 100, 400);
    lv_obj_drag_set_zoom_change_mode(bread_roller, LV_DRAG_GRADIENT, LV_DRAG_ZOOM_CENTER);
    
    lv_obj_drag_set_main_x_offset(bread_roller, 50);

    lv_obj_drag_set_active(bread_roller, 0, LV_ANIM_OFF);
}

void bread_main_menu_start_btn_custom_clicked(void) {
    lv_obj_clear_flag(bread_roller, LV_OBJ_FLAG_CLICKABLE);
}

void bread_main_menu_cancel_btn_custom_clicked(void) {
    lv_obj_add_flag(bread_roller, LV_OBJ_FLAG_CLICKABLE);
}
