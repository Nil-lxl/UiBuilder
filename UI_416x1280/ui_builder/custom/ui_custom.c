#include "custom.h"
#include "ui_objects.h"

static screen_t *scr;

lv_style_t grad_bar_indicator_style;
lv_style_t grad_bar_bg_style;

lv_style_t arc1_indicator_style;

lv_style_t music_bar_indicator_style;

lv_style_t slider_indicator_style;
lv_style_t slider_knob_style;

static void anim_timer_cb(lv_timer_t *timer);
static void num_timer_cb(lv_timer_t *timer);

typedef struct {
    char *type;    //对象类型

    lv_obj_t *obj; //对象指针

    int value;     //当前值

    int direct;    //当前值变化方向，0为增加，1为减少

    int max_value;  //最大值

    int step;       //每次变化的步长

} anim_param_t;

static anim_param_t params[20];

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

    lv_style_init(&music_bar_indicator_style);
    lv_style_set_bg_color(&music_bar_indicator_style, lv_color_hex(0xd237f0));
    // lv_style_set_bg_grad_color(&music_bar_indicator_style, lv_color_hex(0x123dfe));
    // lv_style_set_bg_grad_dir(&music_bar_indicator_style, LV_GRAD_DIR_VER);
    // lv_style_set_bg_grad_stop(&music_bar_indicator_style,255);
    // lv_style_set_bg_main_stop(&music_bar_indicator_style,30);

    /* 默认展开dropdown选项 */
    lv_dropdown_open(scr->dropdown_1);

    /* 弧形条样式 */
    lv_style_init(&arc1_indicator_style);
    lv_style_set_arc_rounded(&arc1_indicator_style, 0);
    lv_style_set_arc_width(&arc1_indicator_style, 30);
    lv_style_set_arc_color(&arc1_indicator_style, lv_color_hex(0xffffff));

    /* 滑动条样式 */
    lv_style_init(&slider_indicator_style);
    lv_style_set_bg_color(&slider_indicator_style, lv_color_hex(0x123dfe));

    lv_style_init(&slider_knob_style);
    lv_style_set_bg_color(&slider_knob_style, lv_color_hex(0xffffff));
    lv_style_set_height(&slider_knob_style, 50);
    lv_style_set_radius(&slider_knob_style, 5);

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

    lv_obj_t *arc1 = scr->arc_1;
    lv_obj_add_style(arc1, &arc1_indicator_style, LV_PART_INDICATOR);
    lv_obj_remove_style(arc1, NULL, LV_PART_KNOB);

    lv_obj_t *music_bar1 = scr->music_bar1;
    lv_obj_add_style(music_bar1, &music_bar_indicator_style, LV_PART_INDICATOR);

    lv_obj_t *music_bar2 = scr->music_bar2;
    lv_obj_add_style(music_bar2, &music_bar_indicator_style, LV_PART_INDICATOR);

    lv_obj_t *music_bar3 = scr->music_bar3;
    lv_obj_add_style(music_bar3, &music_bar_indicator_style, LV_PART_INDICATOR);

    lv_obj_t *music_bar4 = scr->music_bar4;
    lv_obj_add_style(music_bar4, &music_bar_indicator_style, LV_PART_INDICATOR);

    lv_obj_t *music_bar5 = scr->music_bar5;
    lv_obj_add_style(music_bar5, &music_bar_indicator_style, LV_PART_INDICATOR);

    lv_obj_t *music_bar6 = scr->music_bar6;
    lv_obj_add_style(music_bar6, &music_bar_indicator_style, LV_PART_INDICATOR);

    lv_obj_t *slider1 = scr->slider_1;
    lv_obj_add_style(slider1, &slider_indicator_style, LV_PART_INDICATOR);
    lv_obj_add_style(slider1, &slider_knob_style, LV_PART_KNOB);


    params[0] = (anim_param_t){ "bar", grad_bar1, 30, 0, 100, 1 };
    params[1] = (anim_param_t){ "bar", grad_bar2, 70, 0, 100, 1 };
    params[2] = (anim_param_t){ "bar", grad_bar3, 50, 0, 100, 1 };
    params[3] = (anim_param_t){ "bar", grad_bar4, 20, 0, 100, 1 };

    params[4] = (anim_param_t){ "bar", music_bar1, 20, 0, 60, 2 };
    params[5] = (anim_param_t){ "bar", music_bar2, 30, 0, 80, 2 };
    params[6] = (anim_param_t){ "bar", music_bar3, 40, 0, 100, 2 };
    params[7] = (anim_param_t){ "bar", music_bar4, 60, 0, 100, 2 };
    params[8] = (anim_param_t){ "bar", music_bar5, 50, 0, 80, 2 };
    params[9] = (anim_param_t){ "bar", music_bar6, 0, 0, 60, 2 };

    params[10] = (anim_param_t){ "arc", arc1, 20, 0, 100, 1 };

    // params[11] = (anim_param_t){ "slider", slider1, 20, 0, 100, 1 };


    lv_timer_create(anim_timer_cb, 10, NULL);
    lv_timer_create(num_timer_cb, 50, NULL);

}

static void anim_timer_cb(lv_timer_t *timer) {
    for (int i = 0;i < sizeof(params) / sizeof(params[0]);i++) {
        anim_param_t *param = &params[i];

        if (param->obj == NULL) {
            continue;
        }

        if (param->direct == 0) {
            param->value += param->step;
            if (param->value > param->max_value) {
                param->value = param->max_value;
                param->direct = 1;
            }
        } else {
            param->value -= param->step;
            if (param->value < 0) {
                param->value = 0;
                param->direct = 0;
            }
        }
        if (param->type == "bar") {
            lv_bar_set_value(param->obj, param->value, LV_ANIM_OFF);
        } else if (param->type == "arc") {
            lv_arc_set_value(param->obj, param->value);
        } else if (param->type == "slider") {
            lv_slider_set_value(param->obj, param->value, LV_ANIM_OFF);
        }

    }
}

static void num_timer_cb(lv_timer_t *timer) {
    int pct_value = lv_arc_get_value(scr->arc_1);
    lv_label_set_text_fmt(scr->pct_num, "%d%%", pct_value);
    lv_label_set_text_fmt(scr->money_num, "%dk", pct_value * 5);

    int volume_value = lv_slider_get_value(scr->slider_1);
    lv_label_set_text_fmt(scr->volume_label, "volume: %d", volume_value);

}