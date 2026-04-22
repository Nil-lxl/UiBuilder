#include "custom.h"
#include "ui_objects.h"


static screen_t *scr;

lv_style_t grad_bar_indicator_style;
lv_style_t grad_bar_bg_style;


void screen_custom_load_start() {
    scr = screen_get(&ui_manager);

    /* 进度条样式 */
    lv_style_init(&grad_bar_indicator_style);
    lv_style_set_bg_color(&grad_bar_indicator_style, lv_color_hex(0x123dfe));
    lv_style_set_bg_grad_color(&grad_bar_indicator_style, lv_color_hex(0xd237f0));
    lv_style_set_bg_grad_dir(&grad_bar_indicator_style, LV_GRAD_DIR_HOR);

    lv_style_init(&grad_bar_bg_style);
    lv_style_set_border_width(&grad_bar_bg_style, 5);
    lv_style_set_pad_all(&grad_bar_bg_style, 5);

    lv_obj_t *grad_bar1 = scr->grad_bar1;
    lv_obj_add_style(grad_bar1, &grad_bar_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(grad_bar1, &grad_bar_bg_style, LV_PART_MAIN);

    lv_obj_t *grad_bar2 = scr->grad_bar2;
    lv_obj_add_style(grad_bar2, &grad_bar_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(grad_bar2, &grad_bar_bg_style, LV_PART_MAIN);

    lv_obj_t *grad_bar3 = scr->grad_bar3;
    lv_obj_add_style(grad_bar3, &grad_bar_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(grad_bar3, &grad_bar_bg_style, LV_PART_MAIN);

    lv_obj_t *grad_bar4 = scr->grad_bar4;
    lv_obj_add_style(grad_bar4, &grad_bar_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(grad_bar4, &grad_bar_bg_style, LV_PART_MAIN);

}
