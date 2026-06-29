/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"
#include "lvgl.h"
#include <stdbool.h>
#include <stdio.h>

// Define extra image count
#define EXTRA_IMAGE_COUNT 17 

// Global variables
static wallpaper_screen_t *scr = NULL;
static lv_draw_buf_t *snapshot_buf = NULL;  // Snapshot draw buffer (v9)
static lv_obj_t *snapshot_img = NULL;       // Image object for displaying snapshot
static lv_coord_t snapshot_min_x = 0;       // Minimum x coordinate of snapshot
static lv_coord_t snapshot_min_y = 0;       // Minimum y coordinate of snapshot
static lv_coord_t snapshot_width = 0;       // Width of snapshot
static lv_coord_t snapshot_height = 0;      // Height of snapshot

// Array of extra created image objects
static lv_obj_t *extra_images[EXTRA_IMAGE_COUNT] = {NULL};

// Structure to store sub-object information for snapshot to save original position
typedef struct {
    lv_obj_t *child;
    lv_coord_t orig_x;
    lv_coord_t orig_y;
} child_info_t;

// Take a snapshot of all images in the container as one image
static lv_draw_buf_t *create_container_snapshot(lv_obj_t *container)
{
    if (!container) {
        return NULL;
    }

    int child_count = lv_obj_get_child_cnt(container);
    if (child_count == 0) {
        return NULL;
    }

    // Calculate total boundaries of all child objects (using relative coordinates)
    lv_coord_t min_x = 0, min_y = 0, max_x = 0, max_y = 0;
    bool first = true;

    for (int i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(container, i);
        if (!child) continue;  // Add null pointer check
        lv_coord_t x = lv_obj_get_x(child);
        lv_coord_t y = lv_obj_get_y(child);
        lv_coord_t w = lv_obj_get_width(child);
        lv_coord_t h = lv_obj_get_height(child);

        lv_coord_t child_x1 = x;
        lv_coord_t child_y1 = y;
        lv_coord_t child_x2 = x + w - 1;
        lv_coord_t child_y2 = y + h - 1;

        if (first) {
            min_x = child_x1;
            min_y = child_y1;
            max_x = child_x2;
            max_y = child_y2;
            first = false;
        } else {
            if (child_x1 < min_x) min_x = child_x1;
            if (child_y1 < min_y) min_y = child_y1;
            if (child_x2 > max_x) max_x = child_x2;
            if (child_y2 > max_y) max_y = child_y2;
        }
    }
    if (min_x == max_x || min_y == max_y) {
        return NULL;  // Check if boundaries are valid
    }

    lv_coord_t total_width = max_x - min_x + 1;
    lv_coord_t total_height = max_y - min_y + 1;

    // Save boundary information of snapshot for subsequent positioning
    snapshot_min_x = min_x;
    snapshot_min_y = min_y;
    snapshot_width = total_width;
    snapshot_height = total_height;

    // Create temporary container with sufficient size to accommodate all content
    lv_obj_t *temp_container = lv_obj_create(lv_scr_act());
    if (!temp_container) return NULL;
    lv_obj_set_size(temp_container, total_width, total_height);
    lv_obj_set_style_bg_opa(temp_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(temp_container, 0, 0);
    lv_obj_set_style_outline_width(temp_container, 0, 0);
    lv_obj_set_style_pad_all(temp_container, 0, 0);
    lv_obj_add_flag(temp_container, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_pos(temp_container, -1000, -1000); // Move off-screen to avoid display

    // Allocate memory to save child object information
    child_info_t *child_infos = lv_mem_alloc(sizeof(child_info_t) * child_count);
    if (!child_infos) {
        lv_obj_del(temp_container);
        return NULL;
    }

    // Save pointers and position information of all child objects
    for (int i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(container, i);
        if (!child) continue; 
        child_infos[i].child = child;
        child_infos[i].orig_x = lv_obj_get_x(child);
        child_infos[i].orig_y = lv_obj_get_y(child);
    }

    // Move child objects to temporary container
    for (int i = 0; i < child_count; i++) {
        if (!child_infos[i].child) continue;
        lv_obj_t *child = child_infos[i].child;
        lv_coord_t new_x = child_infos[i].orig_x - min_x;
        lv_coord_t new_y = child_infos[i].orig_y - min_y;

        lv_obj_set_parent(child, temp_container);
        lv_obj_set_pos(child, new_x, new_y);
    }

    // Force refresh layout
    lv_obj_update_layout(temp_container);

    // Take snapshot of temporary container
#if LV_USE_SNAPSHOT
    lv_draw_buf_t *full_snapshot = lv_snapshot_take(temp_container, LV_COLOR_FORMAT_ARGB8888);
#else
    lv_draw_buf_t *full_snapshot = NULL;
    printf("Warning: LV_USE_SNAPSHOT is not enabled. Please enable it in lv_conf.h to use snapshot functionality.\n");
#endif

    // Move child objects back to original container and restore positions
    for (int i = 0; i < child_count; i++) {
        if (!child_infos[i].child) continue;

        lv_obj_t *child = child_infos[i].child;
        lv_obj_set_parent(child, container);
        lv_obj_set_pos(child, child_infos[i].orig_x, child_infos[i].orig_y);
    }

    lv_mem_free(child_infos);
    lv_obj_del(temp_container);

    return full_snapshot;
}

// Clean up snapshot resources
static void cleanup_snapshot_resources(void)
{
    if (snapshot_img) {
        lv_img_set_src(snapshot_img, NULL);
        lv_obj_del(snapshot_img);
        snapshot_img = NULL;
    }

    if (snapshot_buf) {
        lv_draw_buf_destroy(snapshot_buf);
        snapshot_buf = NULL;
    }

    // Reset boundary information
    snapshot_min_x = 0;
    snapshot_min_y = 0;
    snapshot_width = 0;
    snapshot_height = 0;
}

// Clean up extra created images
static void cleanup_extra_images(void)
{
    for (int i = 0; i < EXTRA_IMAGE_COUNT; i++) {
        if (extra_images[i] && lv_obj_is_valid(extra_images[i])) {
            lv_obj_del(extra_images[i]);
            extra_images[i] = NULL;
        }
    }
}

// Pre-process click event function, clean resources before lv_obj_clean
static void wallpaper_screen_button_back_clicked_preprocess(lv_event_t *e)
{
    cleanup_snapshot_resources();
    cleanup_extra_images();
}

// Initialize when loading interface
void wallpaper_screen_custom_load_start(void)
{
    scr = wallpaper_screen_get(&ui_manager);
    if (!scr || !scr->obj || !scr->container_1) {
        return;
    }

    // Clean up previous resources
    cleanup_snapshot_resources();

    // Register click event pre-process function
    if (scr->button_1) {
        lv_obj_add_event_cb(scr->button_1,
                            wallpaper_screen_button_back_clicked_preprocess,
                            LV_EVENT_CLICKED | LV_EVENT_PREPROCESS,
                            NULL);
    }

    // Clean up previously created extra images
    cleanup_extra_images();

    // Create 17 additional image controls
    // Existing image positions: image_1(0,0), image_2(200,0), image_3(400,0), image_4(600,0)
    // New images start from (800, 0) and arrange horizontally, alternating between two image sources
    for (int i = 0; i < EXTRA_IMAGE_COUNT; i++) {
        lv_obj_t *img = lv_img_create(scr->container_1);
        if (!img) {
            continue;
        }

        if (i % 2 == 0) {
            lv_img_set_src(img, LVGL_IMAGE_PATH(UI/start/bg_200x200.png));
        } else {
            lv_img_set_src(img, LVGL_IMAGE_PATH(UI/start/bg_2_200x200.png));
        }

        lv_img_set_pivot(img, 50, 50);
        lv_img_set_offset_x(img, 0);
        lv_img_set_offset_y(img, 0);
        lv_img_set_angle(img, 0);
        lv_obj_set_pos(img, 800 + i * 200, 0);

        extra_images[i] = img;
    }

    // Create snapshot
    snapshot_buf = create_container_snapshot(scr->container_1);
    if (!snapshot_buf) {
        cleanup_extra_images();
        printf("Snapshot creation failed\n");
        return;
    }

    // Delete the 17 extra created images after taking the snapshot
    cleanup_extra_images();

    // Hide original images
    if (scr->image_1) lv_obj_add_flag(scr->image_1, LV_OBJ_FLAG_HIDDEN);
    if (scr->image_2) lv_obj_add_flag(scr->image_2, LV_OBJ_FLAG_HIDDEN);
    if (scr->image_3) lv_obj_add_flag(scr->image_3, LV_OBJ_FLAG_HIDDEN);
    if (scr->image_4) lv_obj_add_flag(scr->image_4, LV_OBJ_FLAG_HIDDEN);

    // Disable screen scrolling
    if (scr->obj) {
        lv_obj_set_scroll_dir(scr->obj, LV_DIR_NONE);
        lv_obj_set_scrollbar_mode(scr->obj, LV_SCROLLBAR_MODE_OFF);
    }

    // Create new image object to display snapshot
    snapshot_img = lv_img_create(scr->container_1);
    lv_img_set_src(snapshot_img, snapshot_buf);
    lv_obj_set_pos(snapshot_img, snapshot_min_x, snapshot_min_y);
    lv_obj_set_size(snapshot_img, snapshot_width, snapshot_height);
}

// Clean up when unloading interface
void wallpaper_screen_custom_unloaded(void)
{
    cleanup_snapshot_resources();
    cleanup_extra_images();

    // Restore original image display
    if (!scr) {
        return;
    }

    if (scr->image_1 && lv_obj_is_valid(scr->image_1)) {
        lv_obj_clear_flag(scr->image_1, LV_OBJ_FLAG_HIDDEN);
    }
    if (scr->image_2 && lv_obj_is_valid(scr->image_2)) {
        lv_obj_clear_flag(scr->image_2, LV_OBJ_FLAG_HIDDEN);
    }
    if (scr->image_3 && lv_obj_is_valid(scr->image_3)) {
        lv_obj_clear_flag(scr->image_3, LV_OBJ_FLAG_HIDDEN);
    }
    if (scr->image_4 && lv_obj_is_valid(scr->image_4)) {
        lv_obj_clear_flag(scr->image_4, LV_OBJ_FLAG_HIDDEN);
    }
}
