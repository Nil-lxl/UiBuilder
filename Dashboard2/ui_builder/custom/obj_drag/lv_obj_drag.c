/*
* Copyright (C) 2025, ArtInChip Technology Co., Ltd
*
* SPDX-License-Identifier: Apache-2.0
*
* Authors:  Ning Fang <ning.fang@artinchip.com>
*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_drag.h"
#include "aic_ui.h"
#if LV_USE_OBJ_DRAG !=0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_drag_class)
#define MAX_U32 0xFFFFFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_drag_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_obj_drag_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void drag_child_scroll_ver(lv_obj_t *target, int diff, int index);
static void drag_child_scroll_hor(lv_obj_t *target, int diff, int index);
static void lv_obj_drag_event(const lv_obj_class_t *class_p, lv_event_t *e);
static void lv_obj_drag_del_anim(lv_obj_t *obj);
static void map_y_to_x(lv_obj_t *obj);
static void map_x_to_y(lv_obj_t *obj);
static void set_zoom_change_dir(lv_obj_t *obj, lv_obj_t *child);
static void apply_visual_effects(lv_obj_t *obj, lv_obj_t *target, uint8_t opa, uint32_t zoom, int32_t offset_abs, int32_t child_size);
static void lv_obj_drag_update_child_visual_effect(lv_obj_t *obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_obj_drag_class = {
    .constructor_cb = lv_obj_drag_constructor,
    .destructor_cb = lv_obj_drag_destructor,
    .event_cb = lv_obj_drag_event,
    .width_def = LV_PCT(100),
    .height_def = LV_PCT(100),
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_obj_drag_t),
#if LVGL_VERSION_MAJOR > 8
    .name = "obj_drag",
#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *lv_obj_drag_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_obj_drag_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_obj_drag_set_main_y_offset(lv_obj_t *obj, int32_t main_offset)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->main_y_offset = main_offset;
}

void lv_obj_drag_set_sub_y_offset(lv_obj_t *obj, int32_t sub_offset)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->sub_y_offset = sub_offset;
}

void lv_obj_drag_set_main_x_offset(lv_obj_t *obj, int32_t main_offset)
{
   LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

   obj_drag->main_x_offset = main_offset;
}

void lv_obj_drag_set_sub_x_offset(lv_obj_t *obj, int32_t sub_offset)
{
   LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

   obj_drag->sub_x_offset = sub_offset;
}

void lv_obj_drag_enable_zoom_change(lv_obj_t *obj, bool en, bool only_img)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->zoom_change = en;
    obj_drag->only_img = only_img;
}

void lv_obj_drag_set_zoom_ratio(lv_obj_t *obj, uint32_t zoom_max, uint32_t zoom_min, uint32_t zoom_change_boundary)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->zoom_max = zoom_max;
    obj_drag->zoom_min = zoom_min;
    obj_drag->zoom_change_boundary = zoom_change_boundary;
}

void lv_obj_drag_enable_opa_change(lv_obj_t *obj, bool en, bool only_img)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->opa_change = en;
    obj_drag->only_img = only_img;
}

void lv_obj_drag_set_opa_ratio(lv_obj_t *obj, uint8_t opa_max, uint8_t opa_min, uint32_t opa_change_boundary)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->opa_max = opa_max;
    obj_drag->opa_min = opa_min;
    obj_drag->opa_change_boundary = opa_change_boundary;
}

void lv_obj_drag_enable_transform_mode(lv_obj_t *obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->transform_mode = en;
}

void lv_obj_drag_set_infinite(lv_obj_t *obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->infinite_scroll = en;
}

void lv_obj_drag_set_dir(lv_obj_t *obj, lv_drag_dir_t dir)
{
   LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

   obj_drag->dir = dir;
}

void lv_obj_drag_set_zoom_change_mode(lv_obj_t *obj, lv_drag_change_mode_t mode, lv_drag_zoom_change_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->zoom_change_mode = mode;
    obj_drag->zoom_change_dir = dir;
}

void lv_obj_drag_set_opa_change_mode(lv_obj_t *obj, lv_drag_change_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->opa_change_mode = mode;
}

void lv_obj_drag_add_offset_point(lv_obj_t *obj, lv_point_t *point, uint32_t num)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    if(obj_drag->point)
        lv_free(obj_drag->point);

    obj_drag->point = (lv_point_t *)lv_malloc(sizeof(lv_point_t) * num);
    memcpy(obj_drag->point, point, sizeof(lv_point_t) * num);

    obj_drag->point_num = num;
}

void lv_obj_drag_add_label_item(lv_obj_t *obj, const char *text_list[], int32_t item_cnt, int32_t size_x, \
                                int32_t size_y, const lv_font_t *font, lv_color_t font_color, int32_t font_size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t *child = NULL;

    for(int i = 0; i < item_cnt; i++) {
        child = lv_label_create(obj);
        lv_obj_remove_flag(child, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(child, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_label_set_long_mode(child, LV_LABEL_LONG_WRAP);
        lv_label_set_text(child, text_list[i]);
        lv_obj_set_size(child, size_x, size_y);
        lv_obj_set_style_text_font(child, font, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(child, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(child, (size_y - font_size) / 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(child, font_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align_to(child, obj, LV_ALIGN_TOP_MID, 0, 0);
    }
}

void lv_obj_drag_add_img_item(lv_obj_t *obj, const char *img_list[], int32_t item_cnt, int32_t size_x, int32_t size_y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t *child = NULL;

    for(int i = 0; i < item_cnt; i++) {
        child = lv_img_create(obj);
        lv_obj_remove_flag(child, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(child, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_size(child, size_x, size_y);
        lv_img_set_src(child, img_list[i]);
        lv_img_set_pivot(child, size_x / 2, size_y / 2);
        lv_img_set_angle(child, 0);
        lv_obj_set_style_img_opa(child, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

/*=====================
 * Getter functions
 *====================*/

uint32_t lv_obj_drag_get_child_count(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_obj_get_child_cnt(obj);
}

uint32_t lv_obj_drag_get_active_id(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    return obj_drag->obj_cur;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_obj_drag_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    obj_drag->obj_cur = 0;
    obj_drag->dir = LV_DRAG_DIR_VER;
    obj_drag->obj_last = MAX_U32;
    obj_drag->low_y = 0;
    obj_drag->high_y = 0;
    obj_drag->low_x = 0;
    obj_drag->high_x = 0;
    obj_drag->y_pos = NULL;
    obj_drag->y_pos_num = 0;
    obj_drag->x_pos = NULL;
    obj_drag->x_pos_num = 0;
    obj_drag->main_x_offset = 0;
    obj_drag->sub_x_offset = 0;
    obj_drag->main_y_offset = 0;
    obj_drag->sub_y_offset = 0;
    obj_drag->point = NULL;
    obj_drag->point_num = 0;
    obj_drag->opa_change = false;
    obj_drag->opa_change_mode = 0;
    obj_drag->opa_max = 0;
    obj_drag->opa_min = 0;
    obj_drag->opa_change_boundary = 0;
    obj_drag->zoom_change = false;
    obj_drag->zoom_max = 0;
    obj_drag->zoom_min = 0;
    obj_drag->zoom_change_mode = 0;
    obj_drag->zoom_change_dir = 0;
    obj_drag->zoom_change_boundary = 0;

    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_style_all(obj);
}

static void lv_obj_drag_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    if(obj_drag->y_pos) {
        lv_free(obj_drag->y_pos);
        obj_drag->y_pos = NULL;
    }

    if(obj_drag->x_pos) {
        lv_free(obj_drag->x_pos);
        obj_drag->x_pos = NULL;
    }

    if(obj_drag->point) {
        lv_free(obj_drag->point);
        obj_drag->point = NULL;
    }
}

static void drag_child_scroll_ver(lv_obj_t *target, int diff, int index)
{
    int32_t new_y_val;
    int32_t upper_bound;
    int32_t lower_bound;

    if(target == NULL) return;

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)target;
    lv_obj_t *child = lv_obj_get_child(target, index);

    int32_t y = obj_drag->y_pos[index];
    int32_t low_y = obj_drag->low_y;
    int32_t high_y = obj_drag->high_y;
    float offset_ratio = obj_drag->y_offset_ratio;

    if(diff < 0) {
        if(y <= low_y) {
            new_y_val = y + diff * offset_ratio;
        }
        else if(y > low_y && y <= high_y) {
            if(y + diff >= low_y) {
                new_y_val = y + diff;
            }
            else {
                int diff0 = low_y - y;
                int diff1 = (int)((diff - diff0) * offset_ratio);
                new_y_val = y + diff0 + diff1;
            }
        }
        else { // y > high_y
            if(y + diff * offset_ratio >= high_y) {
                new_y_val = y + diff * offset_ratio;
            }
            else {
                int diff0 = (high_y - y) / offset_ratio;
                int diff1 = diff - diff0;
                if(diff1 >= low_y - high_y) {
                    new_y_val = y + (high_y - y) + diff1;
                }
                else {
                    diff1 = low_y - high_y;
                    int diff2 = (diff - diff0 - diff1) * offset_ratio;
                    new_y_val = y + (high_y - y) + diff1 + diff2;
                }
            }
        }
    }
    else { // diff >= 0
        if(y >= high_y) {
            new_y_val = y + diff * offset_ratio;
        }
        else if(y >= low_y && y < high_y) {
            if(y + diff <= high_y) {
                new_y_val = y + diff;
            }
            else {
                int diff0 = high_y - y;
                int diff1 = (diff - diff0) * offset_ratio;
                new_y_val = y + diff0 + diff1;
            }
        }
        else { // y < low_y
            if(y + diff * offset_ratio <= low_y) {
                new_y_val = y + diff * offset_ratio;
            }
            else {
                int diff0 = (low_y - y) / offset_ratio;
                int diff1 = diff - diff0;
                if(diff1 <= high_y - low_y) {
                    new_y_val = y + (low_y - y) + diff1;
                }
                else {
                    diff1 = high_y - low_y;
                    int diff2 = (diff - diff0 - diff1) * offset_ratio;
                    new_y_val = y + (low_y - y) + diff1 + diff2;
                }
            }
        }
    }

    lv_obj_set_y(child, new_y_val);

    if(obj_drag->infinite_scroll && obj_drag->total_list_height > 0) {
        int32_t parent_height = lv_obj_get_height(target);
        int32_t child_height = lv_obj_get_height(child);

        if(obj_drag->total_list_height > parent_height * 1.5) {
            upper_bound = (obj_drag->total_list_height - parent_height / 2);
            lower_bound = -(obj_drag->total_list_height - parent_height);
        }
        else if(obj_drag->total_list_height == parent_height) {
            upper_bound = parent_height;
            lower_bound = -child_height;
        }
        else {
            upper_bound = parent_height + child_height;
            lower_bound = -child_height * 2;
        }

        int32_t current_y = lv_obj_get_y(child);
        if(current_y > upper_bound) {
            lv_obj_set_y(child, current_y - obj_drag->total_list_height);
            obj_drag->y_pos[index] -= obj_drag->total_list_height;
        }
        else if(current_y < lower_bound) {
            lv_obj_set_y(child, current_y + obj_drag->total_list_height);
            obj_drag->y_pos[index] += obj_drag->total_list_height;
        }
    }

    map_y_to_x(child);
}

static void drag_child_scroll_hor(lv_obj_t *target, int diff, int index)
{
    int32_t new_x_val;
    int32_t upper_bound;
    int32_t lower_bound;

    if(target == NULL) return;

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)target;
    int32_t x = obj_drag->x_pos[index];
    int32_t low_x = obj_drag->low_x;
    int32_t high_x = obj_drag->high_x;
    lv_obj_t *child = lv_obj_get_child(target, index);
    float offset_ratio = obj_drag->x_offset_ratio;

    if(diff < 0) {
        if(x <= low_x) {
            new_x_val = x + diff * offset_ratio;
        }
        else if(x > low_x && x <= high_x) {
            if(x + diff >= low_x) {
                new_x_val = x + diff;
            }
            else {
                int diff0 = low_x - x;
                int diff1 = (int)((diff - diff0) * offset_ratio);
                new_x_val = x + diff0 + diff1;
            }
        }
        else { // x > high_x
            if(x + diff * offset_ratio >= high_x) {
                new_x_val = x + diff * offset_ratio;
            }
            else {
                int diff0 = (high_x - x) / offset_ratio;
                int diff1 = diff - diff0;
                if(diff1 >= low_x - high_x) {
                    new_x_val = x + (high_x - x) + diff1;
                }
                else {
                    diff1 = low_x - high_x;
                    int diff2 = (diff - diff0 - diff1) * offset_ratio;
                    new_x_val = x + (high_x - x) + diff1 + diff2;
                }
            }
        }
    }
    else { // diff >= 0
        if(x >= high_x) {
            new_x_val = x + diff * offset_ratio;
        }
        else if(x >= low_x && x < high_x) {
            if(x + diff <= high_x) {
                new_x_val = x + diff;
            }
            else {
                int diff0 = high_x - x;
                int diff1 = (diff - diff0) * offset_ratio;
                new_x_val = x + diff0 + diff1;
            }
        }
        else { // x < low_x
            if(x + diff * offset_ratio <= low_x) {
                new_x_val = x + diff * offset_ratio;
            }
            else {
                int diff0 = (low_x - x) / offset_ratio;
                int diff1 = diff - diff0;
                if(diff1 <= high_x - low_x) {
                    new_x_val = x + (low_x - x) + diff1;
                }
                else {
                    diff1 = high_x - low_x;
                    int diff2 = (diff - diff0 - diff1) * offset_ratio;
                    new_x_val = x + (low_x - x) + diff1 + diff2;
                }
            }
        }
    }

    lv_obj_set_x(child, new_x_val);

    if(obj_drag->infinite_scroll && obj_drag->total_list_width > 0) {
        int32_t parent_width = lv_obj_get_width(target);
        int32_t child_width = lv_obj_get_width(child);

        if(obj_drag->total_list_width > parent_width * 1.5) {
            upper_bound = (obj_drag->total_list_width - parent_width / 2);
            lower_bound = -(obj_drag->total_list_width - parent_width);
        }
        else if(obj_drag->total_list_width == parent_width) {
            upper_bound = parent_width;
            lower_bound = -child_width;
        }
        else {
            upper_bound = parent_width + child_width;
            lower_bound = -child_width * 2;
        }

        int32_t current_x = lv_obj_get_x(child);
        if(current_x > upper_bound) {
            lv_obj_set_x(child, current_x - obj_drag->total_list_width);
            obj_drag->x_pos[index] -= obj_drag->total_list_width;
        }
        else if(current_x < lower_bound) {
            lv_obj_set_x(child, current_x + obj_drag->total_list_width);
            obj_drag->x_pos[index] += obj_drag->total_list_width;
        }
    }

    map_x_to_y(child);
}

static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_t *parent = lv_obj_get_parent((lv_obj_t *)var);

    lv_obj_set_y(var, v);
    map_y_to_x(var);
    lv_obj_drag_update_child_visual_effect(parent);
}

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_t *parent = lv_obj_get_parent((lv_obj_t *)var);

    lv_obj_set_x(var, v);
    map_x_to_y(var);
    lv_obj_drag_update_child_visual_effect(parent);
}

static void lv_obj_drag_del_anim(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    int32_t child_cnt;
    LV_ASSERT_OBJ(obj, MY_CLASS);

    child_cnt = lv_obj_get_child_cnt(obj);

    for(int32_t i = 0; i < child_cnt; i++) {
        lv_anim_del(lv_obj_get_child(obj, i), NULL);
    }
}

static void lv_obj_drag_anim_completed_cb(lv_anim_t *a)
{
    if((lv_obj_t *)a->var == NULL) return;
    lv_obj_t *parent = lv_obj_get_parent((lv_obj_t *)a->var);

#if LVGL_VERSION_MAJOR == 8
    lv_event_send(parent, LV_EVENT_VALUE_CHANGED, NULL);
#else
    lv_obj_send_event(parent, LV_EVENT_VALUE_CHANGED, NULL);
#endif
}

void lv_obj_drag_set_active(lv_obj_t *obj, uint32_t index, lv_anim_enable_t anim_en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    int duration = 150;

    uint32_t child_cnt = lv_obj_get_child_cnt(obj);
    if(index >= child_cnt)
        return;

    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;
    lv_obj_t *child = lv_obj_get_child(obj, index);

    obj_drag->obj_cur = index;

    if(obj_drag->dir == LV_DRAG_DIR_VER) {
        lv_area_t cont_a;
        lv_obj_get_coords(obj, &cont_a);
        int32_t cont_center = lv_area_get_height(&cont_a) / 2;
        int32_t child_height = lv_obj_get_height(child);
        int cur_y = cont_center - child_height / 2;

        if(!anim_en) {
            lv_obj_set_y(child, cur_y);
            map_y_to_x(child);
        }
        else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, child);
            lv_anim_set_values(&a, lv_obj_get_y_aligned(child), cur_y);
            lv_anim_set_duration(&a, duration);
            lv_anim_set_exec_cb(&a, anim_y_cb);
            lv_anim_set_deleted_cb(&a, lv_obj_drag_anim_completed_cb);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
            lv_anim_start(&a);
        }

        if(obj_drag->infinite_scroll && child_cnt > 1) {
            int32_t inc_y_after = cur_y;
            for(int k = 1; k <= child_cnt / 2; k++) {
                uint32_t child_idx = (index + k) % child_cnt;
                lv_obj_t *sibling = lv_obj_get_child(obj, child_idx);

                if(k == 1) {
                    inc_y_after += child_height + obj_drag->main_y_offset;
                }
                else {
                    inc_y_after += child_height + obj_drag->sub_y_offset;
                }

                int32_t current_sibling_y = lv_obj_get_y_aligned(sibling);
                if(!anim_en || LV_ABS(inc_y_after - current_sibling_y) > obj_drag->total_list_height / 2) {
                    lv_obj_set_y(sibling, inc_y_after);
                    map_y_to_x(sibling);
                }
                else {
                    lv_anim_t a;
                    lv_anim_init(&a);
                    lv_anim_set_var(&a, sibling);
                    lv_anim_set_values(&a, lv_obj_get_y_aligned(sibling), inc_y_after);
                    lv_anim_set_duration(&a, duration);
                    lv_anim_set_exec_cb(&a, anim_y_cb);
                    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                    lv_anim_start(&a);
                }
            }

            int32_t inc_y_before = cur_y;
            for(int k = 1; k <= (child_cnt - 1) / 2; k++) {
                uint32_t child_idx = (index - k + child_cnt) % child_cnt;
                lv_obj_t *sibling = lv_obj_get_child(obj, child_idx);

                if(k == 1) {
                    inc_y_before -= child_height + obj_drag->main_y_offset;
                }
                else {
                    inc_y_before -= child_height + obj_drag->sub_y_offset;
                }

                int32_t current_sibling_y = lv_obj_get_y_aligned(sibling);
                if(!anim_en || LV_ABS(inc_y_before - current_sibling_y) > obj_drag->total_list_height / 2) {
                    lv_obj_set_y(sibling, inc_y_before);
                    map_y_to_x(sibling);
                }
                else {
                    lv_anim_t a;
                    lv_anim_init(&a);
                    lv_anim_set_var(&a, sibling);
                    lv_anim_set_values(&a, lv_obj_get_y_aligned(sibling), inc_y_before);
                    lv_anim_set_duration(&a, duration);
                    lv_anim_set_exec_cb(&a, anim_y_cb);
                    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                    lv_anim_start(&a);
                }
            }
        }
        else {
            if(index < child_cnt - 1) {
                int inc_y = cur_y;
                for(int i = index + 1; i < child_cnt; i++) {
                    child = lv_obj_get_child(obj, i);
                    if(i == index + 1) {
                        inc_y += child_height + obj_drag->main_y_offset;
                    }
                    else {
                        inc_y += child_height + obj_drag->sub_y_offset;
                    }

                    if(!anim_en) {
                        lv_obj_set_y(child, inc_y);
                        map_y_to_x(child);
                    }
                    else {
                        lv_anim_t a;
                        lv_anim_init(&a);
                        lv_anim_set_var(&a, child);
                        lv_anim_set_values(&a, lv_obj_get_y_aligned(child), inc_y);
                        lv_anim_set_duration(&a, duration);
                        lv_anim_set_exec_cb(&a, anim_y_cb);
                        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                        lv_anim_start(&a);
                    }
                }
            }

            if(index > 0) {
                int inc_y = cur_y;
                for(int i = index - 1; i >= 0; i--) {
                    child = lv_obj_get_child(obj, i);
                    if(i == index - 1) {
                        inc_y -= child_height + obj_drag->main_y_offset;
                    }
                    else {
                        inc_y -= child_height + obj_drag->sub_y_offset;
                    }

                    if(!anim_en) {
                        lv_obj_set_y(child, inc_y);
                        map_y_to_x(child);
                    }
                    else {
                        lv_anim_t a;
                        lv_anim_init(&a);
                        lv_anim_set_var(&a, child);
                        lv_anim_set_values(&a, lv_obj_get_y_aligned(child), inc_y);
                        lv_anim_set_duration(&a, duration);
                        lv_anim_set_exec_cb(&a, anim_y_cb);
                        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                        lv_anim_start(&a);
                    }
                }
            }
        }
    }
    else {
        lv_area_t cont_a;
        lv_obj_get_coords(obj, &cont_a);
        int32_t cont_center = lv_area_get_width(&cont_a) / 2;
        int32_t child_width = lv_obj_get_width(child);
        int cur_x = cont_center - child_width / 2;
        if(!anim_en) {
            lv_obj_set_x(child, cur_x);
            map_x_to_y(child);
        }
        else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, child);
            lv_anim_set_values(&a, lv_obj_get_x_aligned(child), cur_x);
            lv_anim_set_duration(&a, duration);
            lv_anim_set_exec_cb(&a, anim_x_cb);
            lv_anim_set_deleted_cb(&a, lv_obj_drag_anim_completed_cb);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
            lv_anim_start(&a);
        }

        if(obj_drag->infinite_scroll && child_cnt > 1) {
            int32_t inc_x_after = cur_x;
            for(int k = 1; k <= child_cnt / 2; k++) {
                uint32_t child_idx = (index + k) % child_cnt;
                lv_obj_t *sibling = lv_obj_get_child(obj, child_idx);
                if(k == 1) {
                    inc_x_after += child_width + obj_drag->main_x_offset;
                }
                else {
                    inc_x_after += child_width + obj_drag->sub_x_offset;
                }

                int32_t current_sibling_x = lv_obj_get_x_aligned(sibling);
                if(!anim_en || LV_ABS(inc_x_after - current_sibling_x) > obj_drag->total_list_width / 2) {
                    lv_obj_set_x(sibling, inc_x_after);
                    map_x_to_y(sibling);
                }
                else {
                    lv_anim_t a;
                    lv_anim_init(&a);
                    lv_anim_set_var(&a, sibling);
                    lv_anim_set_values(&a, lv_obj_get_x_aligned(sibling), inc_x_after);
                    lv_anim_set_duration(&a, duration);
                    lv_anim_set_exec_cb(&a, anim_x_cb);
                    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                    lv_anim_start(&a);
                }
            }

            int32_t inc_x_before = cur_x;
            for(int k = 1; k <= (child_cnt - 1) / 2; k++) {
                uint32_t child_idx = (index - k + child_cnt) % child_cnt;
                lv_obj_t *sibling = lv_obj_get_child(obj, child_idx);
                if(k == 1) {
                    inc_x_before -= child_width + obj_drag->main_x_offset;
                }
                else {
                    inc_x_before -= child_width + obj_drag->sub_x_offset;
                }

                int32_t current_sibling_x = lv_obj_get_x_aligned(sibling);
                if(!anim_en || LV_ABS(inc_x_before - current_sibling_x) > obj_drag->total_list_width / 2) {
                    lv_obj_set_x(sibling, inc_x_before);
                    map_x_to_y(sibling);
                }
                else {
                    lv_anim_t a;
                    lv_anim_init(&a);
                    lv_anim_set_var(&a, sibling);
                    lv_anim_set_values(&a, lv_obj_get_x_aligned(sibling), inc_x_before);
                    lv_anim_set_duration(&a, duration);
                    lv_anim_set_exec_cb(&a, anim_x_cb);
                    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                    lv_anim_start(&a);
                }
            }
        }
        else {
            if(index < child_cnt - 1) {
                int inc_x = cur_x;
                for(int i = index + 1; i < child_cnt; i++) {
                    child = lv_obj_get_child(obj, i);
                    if(i == index + 1) {
                        inc_x += child_width + obj_drag->main_x_offset;
                    }
                    else {
                        inc_x += child_width + obj_drag->sub_x_offset;
                    }

                    if(!anim_en) {
                        lv_obj_set_x(child, inc_x);
                        map_x_to_y(child);
                    }
                    else {
                        lv_anim_t a;
                        lv_anim_init(&a);
                        lv_anim_set_var(&a, child);
                        lv_anim_set_values(&a, lv_obj_get_x_aligned(child), inc_x);
                        lv_anim_set_duration(&a, duration);
                        lv_anim_set_exec_cb(&a, anim_x_cb);
                        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                        lv_anim_start(&a);
                    }
                }
            }

            if(index > 0) {
                int inc_x = cur_x;
                for(int i = index - 1; i >= 0; i--) {
                    child = lv_obj_get_child(obj, i);
                    if(i == index - 1) {
                        inc_x -= child_width + obj_drag->main_x_offset;
                    }
                    else {
                        inc_x -= child_width + obj_drag->sub_x_offset;
                    }

                    if(!anim_en) {
                        lv_obj_set_x(child, inc_x);
                        map_x_to_y(child);
                    }
                    else {
                        lv_anim_t a;
                        lv_anim_init(&a);
                        lv_anim_set_var(&a, child);
                        lv_anim_set_values(&a, lv_obj_get_x_aligned(child), inc_x);
                        lv_anim_set_duration(&a, duration);
                        lv_anim_set_exec_cb(&a, anim_x_cb);
                        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
                        lv_anim_start(&a);
                    }
                }
            }
        }
    }

    lv_obj_drag_update_child_visual_effect(obj);
}

static bool lv_obj_drag_check_region_valid_ver(lv_obj_t *target, int32_t step)
{
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)target;

    int32_t first_y = obj_drag->y_pos[0];

    if(first_y + step > obj_drag->high_y) {
        return false;
    }

    int32_t last_y = obj_drag->y_pos[obj_drag->y_pos_num - 1];;

    if(last_y + step < obj_drag->low_y) {
        return false;
    }

    return true;
}

static bool lv_obj_drag_check_region_valid_hor(lv_obj_t *target, int32_t step)
{
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)target;

    int32_t first_x = obj_drag->x_pos[0];

    if(first_x + step > obj_drag->high_x) {
        return false;
    }

    int32_t last_x = obj_drag->x_pos[obj_drag->x_pos_num - 1];

    if(last_x + step < obj_drag->low_x) {
        return false;
    }

    return true;
}

static void lv_obj_drag_event(const lv_obj_class_t *class_p, lv_event_t *e)
{
    LV_UNUSED(class_p);

    lv_obj_t *target = lv_event_get_current_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)target;
    lv_res_t res = lv_obj_event_base(&lv_obj_drag_class, e);

    if(res != LV_RES_OK)
        return;

    lv_indev_t *indev = lv_indev_active();
    if(indev == NULL)
        return;

    if(code == LV_EVENT_PRESSED) {
        int32_t child_cnt = lv_obj_get_child_cnt(target);
        lv_indev_get_point(indev, &obj_drag->last_point);

        lv_obj_drag_del_anim(target);

        if(obj_drag->dir == LV_DRAG_DIR_VER) {
            if(obj_drag->y_pos_num != child_cnt) {
                if(obj_drag->y_pos) {
                    lv_free(obj_drag->y_pos);
                    obj_drag->y_pos = NULL;
                }
                if(child_cnt > 0) {
                    obj_drag->y_pos = lv_malloc(sizeof(int32_t) * child_cnt);
                }
                obj_drag->y_pos_num = child_cnt;
            }
            if(child_cnt > 0) {
                lv_obj_t *child = lv_obj_get_child(target, 0);
                int32_t parent_height = lv_obj_get_height(target);
                int32_t child_height = lv_obj_get_height(child);
                obj_drag->low_y = parent_height / 2 - obj_drag->main_y_offset - child_height * 3 / 2;
                obj_drag->high_y = parent_height / 2 + obj_drag->main_y_offset + child_height / 2;
                if(obj_drag->main_y_offset + child_height > 0) {
                    obj_drag->y_offset_ratio = (float)(obj_drag->sub_y_offset + child_height) / (float)(obj_drag->main_y_offset + child_height);
                }

                if(obj_drag->infinite_scroll) {
                    if(child_cnt > 2) {
                        obj_drag->total_list_height = child_cnt * child_height + \
                                                    2 * obj_drag->main_y_offset + \
                                                    (child_cnt - 2) * obj_drag->sub_y_offset;
                    }
                    else if(child_cnt > 1){
                        obj_drag->total_list_height = 2 * child_height + 2 * obj_drag->main_y_offset;
                    }
                    else {
                        obj_drag->total_list_height = 0;
                    }
                }
                for(int32_t i = 0; i < child_cnt; i++) {
                    lv_obj_t *child = lv_obj_get_child(target, i);
                    obj_drag->y_pos[i] = lv_obj_get_y_aligned(child);
                }
            }
        }
        else { // LV_DRAG_DIR_HOR
            if(obj_drag->x_pos_num != child_cnt) {
                if(obj_drag->x_pos) {
                    lv_free(obj_drag->x_pos);
                    obj_drag->x_pos = NULL;
                }
                if(child_cnt > 0) {
                    obj_drag->x_pos = lv_malloc(sizeof(int32_t) * child_cnt);
                }
                obj_drag->x_pos_num = child_cnt;
            }
            if(child_cnt > 0) {
                lv_obj_t *child = lv_obj_get_child(target, 0);
                int32_t parent_width = lv_obj_get_width(target);
                int32_t child_width = lv_obj_get_width(child);
                obj_drag->low_x = parent_width / 2 - obj_drag->main_x_offset - child_width * 3 / 2;
                obj_drag->high_x = parent_width / 2 + obj_drag->main_x_offset + child_width / 2;
                if(obj_drag->main_x_offset + child_width > 0) {
                    obj_drag->x_offset_ratio = (float)(obj_drag->sub_x_offset + child_width) / (float)(obj_drag->main_x_offset + child_width);
                }

                if(obj_drag->infinite_scroll) {
                    if(child_cnt > 2) {
                        obj_drag->total_list_width = child_cnt * child_width + \
                                                    2 * obj_drag->main_x_offset + \
                                                    (child_cnt - 2) * obj_drag->sub_x_offset;
                    }
                    else if(child_cnt > 1) {
                        obj_drag->total_list_width = 2 * child_width + 2 * obj_drag->main_x_offset;
                    }
                    else {
                        obj_drag->total_list_width = 0;
                    }
                }
                for(int32_t i = 0; i < child_cnt; i++) {
                    lv_obj_t *child = lv_obj_get_child(target, i);
                    obj_drag->x_pos[i] = lv_obj_get_x_aligned(child);
                }
            }
        }
    }
    else if(code == LV_EVENT_PRESSING) {
        lv_point_t act_point;
        lv_indev_get_point(lv_indev_get_act(), &act_point);
        int32_t child_cnt = lv_obj_get_child_cnt(target);
        if(child_cnt == 0)
            return;

        bool can_drag = true;
        if(obj_drag->dir == LV_DRAG_DIR_VER) {
            int inc_step = act_point.y - obj_drag->last_point.y;

            if(!obj_drag->infinite_scroll) {
                can_drag = lv_obj_drag_check_region_valid_ver(target, inc_step);
            }

            if(can_drag) {
                for(int32_t i = 0; i < child_cnt; i++) {
                    drag_child_scroll_ver(target, inc_step, i);
                }
            }
        }
        else { // LV_DRAG_DIR_HOR
            int inc_step = act_point.x - obj_drag->last_point.x;

            if(!obj_drag->infinite_scroll) {
                can_drag = lv_obj_drag_check_region_valid_hor(target, inc_step);
            }

            if(can_drag) {
                for(int32_t i = 0; i < child_cnt; i++) {
                    drag_child_scroll_hor(target, inc_step, i);
                }
            }
        }
        lv_obj_drag_update_child_visual_effect(target);
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        int32_t child_cnt = lv_obj_get_child_cnt(target);
        if(child_cnt == 0) return;

        int min_index = 0;
        int32_t min_value = -1;
        lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)target;

        if(obj_drag->dir == LV_DRAG_DIR_VER) {
            lv_area_t cont_a;
            lv_obj_get_coords(target, &cont_a);
            int32_t cont_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

            for(int32_t i = 0; i < child_cnt; i++) {
                lv_obj_t * child = lv_obj_get_child(target, i);
                lv_area_t child_a;
                lv_obj_get_coords(child, &child_a);
                int32_t child_center = child_a.y1 + lv_area_get_height(&child_a) / 2;
                int32_t diff = LV_ABS(child_center - cont_center);

                if(min_value == -1 || diff < min_value) {
                    min_value = diff;
                    min_index = i;
                }
            }
        }
        else { // LV_DRAG_DIR_HOR
            lv_area_t cont_a;
            lv_obj_get_coords(target, &cont_a);
            int32_t cont_center = cont_a.x1 + lv_area_get_width(&cont_a) / 2;

            for(int32_t i = 0; i < child_cnt; i++) {
                lv_obj_t * child = lv_obj_get_child(target, i);
                lv_area_t child_a;
                lv_obj_get_coords(child, &child_a);
                int32_t child_center = child_a.x1 + lv_area_get_width(&child_a) / 2;
                int32_t diff = LV_ABS(child_center - cont_center);

                if(min_value == -1 || diff < min_value) {
                    min_value = diff;
                    min_index = i;
                }
            }
        }

        lv_obj_drag_set_active(target, min_index, LV_ANIM_ON);
    }
}

static void map_y_to_x(lv_obj_t *obj)
{

    int start_x = 0;
    int start_y = 0;
    int end_x = 0;
    int end_y = 0;
    int cur_x = 0;
    float offset_ratio = 0;
    bool x_ok = false;
    int i;

    lv_obj_t *parent = lv_obj_get_parent(obj);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)parent;


    int32_t parent_height = lv_obj_get_height(parent);
    int32_t child_height = lv_obj_get_height(obj);
    int32_t center = parent_height / 2;
    int32_t child_center = lv_obj_get_y_aligned(obj) + child_height / 2;
    int32_t y_offset = child_center - center;
    int32_t y_offset_abs = LV_ABS(y_offset);

    if(obj == NULL || obj_drag->transform_mode == false)
        return;

    if(obj_drag->point_num < 2)
        return;

    for(i = 0; i < obj_drag->point_num; i++) {
        if(y_offset_abs == obj_drag->point[i].y) {
            cur_x = obj_drag->point[i].x;
            x_ok = true;
            break;
        }

        if(y_offset_abs <= obj_drag->point[i].y) {
            break;
        }
    }

    if(x_ok == false) {
        if(i < obj_drag->point_num) {
            start_x = obj_drag->point[i - 1].x;
            start_y = obj_drag->point[i - 1].y;
            end_x = obj_drag->point[i].x;
            end_y = obj_drag->point[i].y;
            offset_ratio = (float)(end_x - start_x) / (float)(end_y - start_y);
            cur_x = start_x + offset_ratio * (y_offset_abs - start_y);
        }
        else {
            start_x = obj_drag->point[i - 2].x;
            start_y = obj_drag->point[i - 2].y;
            end_x = obj_drag->point[i - 1].x;
            end_y = obj_drag->point[i - 1].y;
            offset_ratio = (float)(end_x - start_x) / (float)(end_y - start_y);
            cur_x = end_x + offset_ratio * (y_offset_abs - end_y);
        }
    }

    lv_obj_set_x(obj, -cur_x);
}

static void map_x_to_y(lv_obj_t *obj)
{
    int start_y_pos = 0;
    int start_x_offset = 0;
    int end_y_pos = 0;
    int end_x_offset = 0;
    float ratio = 0;
    int cur_y = 0;
    bool y_ok = false;
    int i;

    lv_obj_t *parent = lv_obj_get_parent(obj);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)parent;
    int32_t parent_width = lv_obj_get_width(parent);
    int32_t child_width = lv_obj_get_width(obj);
    int32_t center = parent_width / 2;
    int32_t child_center = lv_obj_get_x_aligned(obj) + child_width / 2;
    int32_t x_offset = child_center - center;
    int32_t x_offset_abs = LV_ABS(x_offset);

    if(obj == NULL || obj_drag->transform_mode == false)
        return;

    if(obj_drag->point_num < 2) {
        return;
    }


    for(i = 0; i < obj_drag->point_num; i++) {
        if(x_offset_abs == obj_drag->point[i].y) {
            cur_y = obj_drag->point[i].x;
            y_ok = true;
            break;
        }
        if(x_offset_abs < obj_drag->point[i].y) {
            break;
        }
    }

    if(y_ok == false) {
        if(i > 0 && i < obj_drag->point_num) {
            start_y_pos = obj_drag->point[i - 1].x;
            start_x_offset = obj_drag->point[i - 1].y;
            end_y_pos = obj_drag->point[i].x;
            end_x_offset = obj_drag->point[i].y;
            ratio = (float)(end_y_pos - start_y_pos) / (float)(end_x_offset - start_x_offset);
            cur_y = start_y_pos + ratio * (x_offset_abs - start_x_offset);
        }
        else if(i >= obj_drag->point_num && obj_drag->point_num > 1) {
            start_y_pos = obj_drag->point[i - 2].x;
            start_x_offset = obj_drag->point[i - 2].y;
            end_y_pos = obj_drag->point[i - 1].x;
            end_x_offset = obj_drag->point[i - 1].y;
            ratio = (float)(end_y_pos - start_y_pos) / (float)(end_x_offset - start_x_offset);
            cur_y = end_y_pos + ratio * (x_offset_abs - end_x_offset);
        }
    }

    lv_obj_set_y(obj, cur_y);
}

static void set_zoom_change_dir(lv_obj_t *obj, lv_obj_t *child)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    if(obj_drag->only_img) {
        switch(obj_drag->zoom_change_dir) {
            case LV_DRAG_ZOOM_CENTER :
                lv_img_set_pivot(child, lv_obj_get_width(child) / 2, lv_obj_get_height(child) / 2);
                break;
            case LV_DRAG_ZOOM_ORIGIN :
                lv_img_set_pivot(child, 0, 0);
                break;
            case LV_DRAG_ZOOM_LEFT :
                lv_img_set_pivot(child, 0, lv_obj_get_height(child) / 2);
                break;
            case LV_DRAG_ZOOM_RIGHT :
                lv_img_set_pivot(child, lv_obj_get_width(child), lv_obj_get_height(child) / 2);
                break;
            case LV_DRAG_ZOOM_TOP :
                lv_img_set_pivot(child, lv_obj_get_width(child) / 2, 0);
                break;
            case LV_DRAG_ZOOM_BOTTOM :
                lv_img_set_pivot(child, lv_obj_get_width(child) / 2, lv_obj_get_height(child));
                break;
        }
    }
    else {
        switch(obj_drag->zoom_change_dir) {
            case LV_DRAG_ZOOM_CENTER :
                lv_obj_set_style_transform_pivot_x(child, lv_obj_get_width(child) / 2, 0);
                lv_obj_set_style_transform_pivot_y(child, lv_obj_get_height(child) / 2, 0);
                break;
            case LV_DRAG_ZOOM_ORIGIN :
                lv_obj_set_style_transform_pivot_x(child, 0, 0);
                lv_obj_set_style_transform_pivot_y(child, 0, 0);
                break;
            case LV_DRAG_ZOOM_LEFT :
                lv_obj_set_style_transform_pivot_x(child, 0, 0);
                lv_obj_set_style_transform_pivot_y(child, lv_obj_get_height(child) / 2, 0);
                break;
            case LV_DRAG_ZOOM_RIGHT :
                lv_obj_set_style_transform_pivot_x(child, lv_obj_get_width(child), 0);
                lv_obj_set_style_transform_pivot_y(child, lv_obj_get_height(child) / 2, 0);
                break;
            case LV_DRAG_ZOOM_TOP :
                lv_obj_set_style_transform_pivot_x(child, lv_obj_get_width(child) / 2, 0);
                lv_obj_set_style_transform_pivot_y(child, 0, 0);
                break;
            case LV_DRAG_ZOOM_BOTTOM :
                lv_obj_set_style_transform_pivot_x(child, lv_obj_get_width(child) / 2, 0);
                lv_obj_set_style_transform_pivot_y(child, lv_obj_get_height(child), 0);
                break;
        }
    }
}

static void apply_visual_effects(lv_obj_t *obj, lv_obj_t *target, uint8_t opa, uint32_t zoom, int32_t offset_abs, int32_t child_size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    if(obj_drag->opa_change) {
        if(obj_drag->only_img) {
            switch(obj_drag->opa_change_mode) {
                case LV_DRAG_GRADIENT:
                    lv_obj_set_style_img_opa(target, opa, 0);
                    break;
                case LV_DRAG_SOLID:
                    lv_obj_set_style_img_opa(target, (offset_abs <= obj_drag->opa_change_boundary) ? obj_drag->opa_max : obj_drag->opa_min, 0);
                    break;
            }
        }
        else {
            switch(obj_drag->opa_change_mode) {
                case LV_DRAG_GRADIENT:
                    lv_obj_set_style_opa(target, opa, 0);
                    break;
                case LV_DRAG_SOLID:
                    lv_obj_set_style_opa(target, (offset_abs <= obj_drag->opa_change_boundary) ? obj_drag->opa_max : obj_drag->opa_min, 0);
                    break;
            }
        }
    }

    if(obj_drag->zoom_change) {
        set_zoom_change_dir(obj, target);
        if(obj_drag->only_img) {
            switch(obj_drag->zoom_change_mode) {
                case LV_DRAG_GRADIENT:
                    lv_img_set_zoom(target, zoom);
                    break;
                case LV_DRAG_SOLID:
                    lv_img_set_zoom(target, (offset_abs <= obj_drag->zoom_change_boundary) ? obj_drag->zoom_max : obj_drag->zoom_min);
                    break;
            }
        }
        else {
            switch(obj_drag->zoom_change_mode) {
                case LV_DRAG_GRADIENT:
                    lv_obj_set_style_transform_zoom(target, zoom, 0);
                    break;
                case LV_DRAG_SOLID:
                    lv_obj_set_style_transform_zoom(target, (offset_abs <= obj_drag->zoom_change_boundary) ? obj_drag->zoom_max : obj_drag->zoom_min, 0);
                    break;
            }
        }
    }
}

static void lv_obj_drag_update_child_visual_effect(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_drag_t *obj_drag = (lv_obj_drag_t *)obj;

    uint32_t child_cnt = lv_obj_get_child_cnt(obj);
    if(child_cnt == 0) {
        return;
    }

    bool is_vertical = (obj_drag->dir == LV_DRAG_DIR_VER);
    int32_t parent_size = is_vertical ? lv_obj_get_height(obj) : lv_obj_get_width(obj);
    int32_t center = parent_size / 2;

    if(obj_drag->opa_change_boundary > center) {
        obj_drag->opa_change_boundary = center;
    }
    if(obj_drag->zoom_change_boundary > center) {
        obj_drag->zoom_change_boundary = center;
    }

    for(int i = 0; i < child_cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(obj, i);

        int32_t child_size = is_vertical ? lv_obj_get_height(child) : lv_obj_get_width(child);
        int32_t child_center = (is_vertical ? lv_obj_get_y_aligned(child) : lv_obj_get_x_aligned(child)) + (child_size / 2);

        int32_t offset = child_center - center;
        int32_t offset_abs = LV_ABS(offset);

        if(offset_abs > center) {
            offset_abs = center;
        }

        uint8_t opa = lv_map(offset_abs, 0, obj_drag->opa_change_boundary, obj_drag->opa_max, obj_drag->opa_min);
        uint32_t zoom = lv_map(offset_abs, 0, obj_drag->zoom_change_boundary, obj_drag->zoom_max, obj_drag->zoom_min);

        uint32_t child_sub_cnt = lv_obj_get_child_cnt(child);
        if(child_sub_cnt > 0) {
            for(int m = 0; m < child_sub_cnt; m++) {
                lv_obj_t *child_sub = lv_obj_get_child(child, m);
                apply_visual_effects(obj, child_sub, opa, zoom, offset_abs, child_size);
            }
        }
        else {
            apply_visual_effects(obj, child, opa, zoom, offset_abs, child_size);
        }
    }
}

#endif /*LV_USE_OBJ_DRAG*/
