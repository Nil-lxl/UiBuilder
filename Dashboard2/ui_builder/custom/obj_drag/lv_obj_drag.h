/*
 * Copyright (C) 2025, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Ning Fang <ning.fang@artinchip.com>
 */

#ifndef LV_OBJ_DRAG
#define LV_OBJ_DRAG

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

#ifndef LV_USE_OBJ_DRAG
#define LV_USE_OBJ_DRAG 1
#endif

#if LV_USE_OBJ_DRAG

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAG_DIR_VER,        /*< Child object will arrenged horizontally. */
    LV_DRAG_DIR_HOR         /*< Child object will arrenged vertically. */ 
} lv_drag_dir_t;

typedef enum {
    LV_DRAG_GRADIENT,       /*< The opa of child object will change gradually. */
    LV_DRAG_SOLID           /*< Only active child object displayed with opa_max, others displayed with opa_min. */
} lv_drag_change_mode_t;

typedef enum {
    LV_DRAG_ZOOM_CENTER,      /*< Center point as scaling reference. */
    LV_DRAG_ZOOM_ORIGIN,      /*< Coordinate system origin (0,0) based scaling. */
    LV_DRAG_ZOOM_LEFT,        /*< Fix left edge while scaling horizontally. */
    LV_DRAG_ZOOM_RIGHT,       /*< Fix right edge while scaling horizontally. */
    LV_DRAG_ZOOM_TOP,         /*< Fix top edge while scaling vertically. */
    LV_DRAG_ZOOM_BOTTOM       /*< Fix bottom edge while scaling vertically. */
} lv_drag_zoom_change_dir_t;

typedef struct {
    lv_obj_t obj;                       /*< Base LVGL object. */
    uint32_t obj_cur;                   /*< Index of the currently centered or active child object. */
    uint32_t obj_last;                  /*< Index of the previously active child object (reserved). */
    uint32_t obj_cnt;                   /*< Total number of child objects. */

    lv_drag_dir_t dir;                  /*< Drag direction (LV_DRAG_DIR_VER / LV_DRAG_DIR_HOR). */
    bool infinite_scroll;               /*< Enable/disable infinite loop scrolling. */
    bool only_img;                      /*< Apply visual effects only to image parts (lv_img_t). */
    lv_point_t last_point;              /*< Last pressed point, used to calculate drag delta. */

    int32_t *y_pos;                     /*< Array to store the Y coordinates of all child objects for drag calculation. */
    int32_t y_pos_num;                  /*< Number of elements in the y_pos array. */
    int32_t low_y;                      /*< Upper boundary for vertical scrolling (for elastic effect). */
    int32_t high_y;                     /*< Lower boundary for vertical scrolling (for elastic effect). */
    int32_t main_y_offset;              /*< Vertical spacing for the main item (adjacent to center). */
    int32_t sub_y_offset;               /*< Vertical spacing for secondary items. */
    float y_offset_ratio;               /*< Scroll ratio for the elastic area beyond the boundary. */
    int32_t total_list_height;          /*< Total height of the list, used for infinite scroll calculation. */

    int32_t *x_pos;                     /*< Array to store the X coordinates of all child objects for drag calculation. */
    uint32_t x_pos_num;                 /*< Number of elements in the x_pos array. */
    int32_t low_x;                      /*< Left boundary for horizontal scrolling (for elastic effect). */
    int32_t high_x;                     /*< Right boundary for horizontal scrolling (for elastic effect). */
    int32_t main_x_offset;              /*< Horizontal spacing for the main item (adjacent to center). */
    int32_t sub_x_offset;               /*< Horizontal spacing for secondary items. */
    float x_offset_ratio;               /*< Scroll ratio for the elastic area beyond the boundary. */
    int32_t total_list_width;           /*< Total width of the list, used for infinite scroll calculation. */

    bool transform_mode;                /*< Enable/disable coordinate transformation mode. */
    lv_point_t *point;                  /*< Array of key points for the transformation path. */
    int32_t point_num;                  /*< Number of key points in the path. */

    bool opa_change;                    /*< Enable/disable opacity change effect. */
    uint8_t opa_min;                    /*< Minimum opacity (usually for edge items). */
    uint8_t opa_max;                    /*< Maximum opacity (usually for the center item). */
    lv_drag_change_mode_t opa_change_mode; /**< Opacity change mode (LV_DRAG_GRADIENT / LV_DRAG_SOLID). */
    int32_t opa_change_boundary;        /*< Distance between the center of active object and center of the minimum object. */

    bool zoom_change;                   /*< Enable/disable zoom change effect. */
    uint32_t zoom_min;                   /*< Minimum zoom value. */
    uint32_t zoom_max;                   /*< Maximum zoom value. */
    lv_drag_change_mode_t zoom_change_mode; /*< Zoom change mode (LV_DRAG_GRADIENT / LV_DRAG_SOLID). */
    lv_drag_zoom_change_dir_t zoom_change_dir; /**< Transformation pivot for the zoom effect (e.g., center, left, top). */
    int32_t zoom_change_boundary;       /*< Distance between the center of active object and center of the minimum object. */

} lv_obj_drag_t;

 
#if LVGL_VERSION_MAJOR > 8
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_obj_drag_class;
#else
extern const lv_obj_class_t lv_obj_drag_class;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**     
 * Create a drag object.
 * @param parent    pointer to an object, it will be the parent of the new drag.
 * @return          pointer to the created drag.
 */
lv_obj_t *lv_obj_drag_create(lv_obj_t *parent);

/*=====================
 * Getter functions
 *====================*/

/**     
 * Get the total number of children in the drag object.
 * @param obj    pointer to a drag object.
 * @return       the number of child objects.
 */
uint32_t lv_obj_drag_get_child_count(lv_obj_t *obj);

/** 
 * Get the index of the active child in the drag
 * @param obj    pointer to a drag object.
 * @return       the index of active child 
 */
uint32_t lv_obj_drag_get_active_id(lv_obj_t *obj);

/*=====================
* Setter functions
*====================*/

/**
 * Set the vertical gap active child object between adjacent child object (only dir = LV_DRAG_DIR_VER). 
 * @param obj           pointer to a drag object.
 * @param main_offset   the vertical gap value.
 */
void lv_obj_drag_set_main_y_offset(lv_obj_t *obj, int32_t main_offset);

/**
 * Set the vertical gap between adjacent unselected child object (only dir = LV_DRAG_DIR_VER). 
 * @param obj           pointer to a drag object.
 * @param sub_offset    the vertical gap value.
 */
void lv_obj_drag_set_sub_y_offset(lv_obj_t *obj, int32_t sub_offset);

/**
 * Set the horizontal gap active child object between adjacent child object (only dir = LV_DRAG_DIR_HOR).
 * @param obj           pointer to a drag object. 
 * @param main_offset   the horizontal gap value.
 */
void lv_obj_drag_set_main_x_offset(lv_obj_t *obj, int32_t main_offset);

/**
 * Set the horizontal gap between adjacent unselected child object (only dir = LV_DRAG_DIR_HOR). 
 * @param obj           pointer to a drag object. 
 * @param sub_offset    the horizontal gap value.
 */
void lv_obj_drag_set_sub_x_offset(lv_obj_t *obj, int32_t main_offset);

/**
 * Set the specified child item as active (centered).
 * @param obj       pointer to a drag object.
 * @param index     the index of the child item to be set as active.
 * @param anim_en   whether to use an animation (LV_ANIM_ON or LV_ANIM_OFF).
 */
void lv_obj_drag_set_active(lv_obj_t *obj, uint32_t index, lv_anim_enable_t anim_en);

/**
 * Set the parameters for the opacity effect.
 * @param obj                   pointer to a drag object.
 * @param opa_max               the maximum opacity value (255 = 100%).
 * @param opa_min               the minimum opacity value.
 * @param opa_change_boundary   the distance from the center where the opacity effect begins to apply.
 */
void lv_obj_drag_set_opa_ratio(lv_obj_t *obj, uint8_t opa_max, uint8_t opa_min, uint32_t opa_change_boundary);

/**
 * Set the parameters for the zoom effect.
 * @param obj                   pointer to a drag object.
 * @param zoom_max              the maximum zoom value (256 = 100%).
 * @param zoom_min              the minimum zoom value.
 * @param zoom_change_boundary  The distance from the center where the zoom effect begins to apply.
 */
void lv_obj_drag_set_zoom_ratio(lv_obj_t *obj, uint32_t zoom_max, uint32_t zoom_min, uint32_t zoom_change_boundary);

/**
 * Enable or disable infinite scrolling.
 * @param obj   pointer to a drag object.
 * @param en    true to enable, false to disable.
 */
void lv_obj_drag_set_infinite(lv_obj_t *obj, bool en);

/**
 * Set the drag direction for the component.
 * @param obj   pointer to a drag object.
 * @param dir   the drag direction (LV_DRAG_DIR_VER or LV_DRAG_DIR_HOR).
 */
void lv_obj_drag_set_dir(lv_obj_t *obj, lv_drag_dir_t dir);

/**
 * Set the zoom effect's change mode and transformation pivot.
 * @param obj   pointer to a drag object.
 * @param mode  the change mode (gradient or solid).
 * @param dir   the transformation pivot for the zoom.
 */
void lv_obj_drag_set_zoom_change_mode(lv_obj_t *obj, lv_drag_change_mode_t mode, lv_drag_zoom_change_dir_t dir);

/**
 * Set the opacity effect's change mode.
 * @param obj   pointer to a drag object.
 * @param mode  the change mode (gradient or solid).
 */
void lv_obj_drag_set_opa_change_mode(lv_obj_t *obj, lv_drag_change_mode_t mode);

/*=====================
 * Other functions
 *====================*/

/**
 * Add path points for the coordinate transformation mode.
 * @param obj       pointer to a drag object.
 * @param point     pointer to an array of lv_point_t.
 * @param num       the number of points in the array.
 */
void lv_obj_drag_add_offset_point(lv_obj_t *obj, lv_point_t *point, uint32_t num);

/**
 * Enable or disable the opacity visual effect.
 * @param obj       pointer to a drag object.
 * @param en        true to enable, false to disable.
 * @param only_img  true to apply the effect only to image objects, false to apply to the entire child object.
 */
void lv_obj_drag_enable_opa_change(lv_obj_t *obj, bool en, bool only_img);

/**
 * Enable or disable the coordinate transformation mode.
 * @param obj   pointer to a drag object.
 * @param en    true to enable, false to disable.
 */
void lv_obj_drag_enable_transform_mode(lv_obj_t *obj, bool en);

/**
 * Enable or disable the zoom visual effect.
 * @param obj       pointer to a drag object.
 * @param en        true to enable, false to disable.
 * @param only_img  true to apply the effect only to image objects, false to apply to the entire child object.
 */
void lv_obj_drag_enable_zoom_change(lv_obj_t *obj, bool en, bool only_img);

/**
 * Add multiple label objects as children.
 * @param obj           pointer to a drag object.
 * @param text_list     array of text strings.
 * @param item_cnt      the number of items to add.
 * @param size_x        the width of each label.
 * @param size_y        the height of each label.
 * @param font          the font to be used for the labels.
 * @param font_color    the color of the font.
 * @param font_size     the size of the font, used for vertical alignment calculation.
 */
void lv_obj_drag_add_label_item(lv_obj_t *obj, const char *text_list[], int32_t item_cnt, int32_t size_x, \
                            int32_t size_y, const lv_font_t *font, lv_color_t font_color, int32_t font_size);

/**
 * Add multiple image objects as children.
 * @param obj           pointer to a drag object.
 * @param img_list      array of image source strings (paths or variables).
 * @param item_cnt      the number of items to add.
 * @param size_x        the width of each image.
 * @param size_y        the height of each image.
 */
void lv_obj_drag_add_img_item(lv_obj_t *obj, const char *img_list[], int32_t item_cnt, int32_t size_x, int32_t size_y);

#endif /*LV_USE_OBJ_DRAG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_OBJ_DRAG */
