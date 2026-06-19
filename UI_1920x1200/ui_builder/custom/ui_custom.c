#include "custom.h"
#include "ui_objects.h"

static scrMain_t *scrMain;
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


void scrSmartHome_custom_load_start() {
    scrSmartHome = scrSmartHome_get(&ui_manager);

    lv_obj_t *cont_main = scrSmartHome->cont_main;
    lv_obj_set_flex_flow(cont_main, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont_main, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_SPACE_BETWEEN);

    lv_obj_set_style_margin_hor(scrSmartHome->cont_1, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_2, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_3, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_4, 20, 0);
    lv_obj_set_style_margin_hor(scrSmartHome->cont_5, 20, 0);

}