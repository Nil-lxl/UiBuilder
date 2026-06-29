/*
 * Copyright (C) 2026 ArtInChip Technology Co., Ltd.
 *
 */

#include "custom.h"
#include <time.h>

/* ========== Macro Definitions ========== */

#define ALBUM_COUNT       3
#define SKIP_TIME_MS      15000
#define UPDATE_PERIOD_MS  1000
#define NAV_STATE_COUNT   3

/* ========== Type Definitions ========== */

typedef struct {
    uint32_t song_duration_ms;
    uint32_t current_progress_ms;
    uint8_t  is_playing;
    uint8_t  current_song_idx;
    uint8_t  is_initialized;
} music_player_state_t;

/* ========== Static Data ========== */

// Driving distance data
static uint8_t s_data_index = 0;
static const char *s_data_values[] = {"510", "420", "380", "350", "290"};
static uint8_t s_data_value_count = 5;

// Album images
static const char *album_images[] = {
    LVGL_IMAGE_PATH(main/play/album_cover.png),
    LVGL_IMAGE_PATH(main/play/album2.png),
    LVGL_IMAGE_PATH(main/play/album3.png)
};

// Navigation text
static const char *nav_texts[] = {
    "Go straight on Zhuhai Bridge",
    "Turn left onto Hong Kong Rd",
    "Turn right onto Macau Ave"
};

/* ========== Static Variables ========== */

// Music playback
static lv_timer_t *music_timer = NULL;
static lv_obj_t *bar_progress = NULL;
static lv_obj_t *image_album = NULL;
static lv_obj_t *image_start = NULL;
static lv_obj_t *image_stop = NULL;
static lv_obj_t *image_forward = NULL;
static lv_obj_t *image_back = NULL;
static lv_obj_t *image_next = NULL;
static lv_obj_t *image_pre = NULL;
static lv_obj_t *label_time_start = NULL;
static lv_obj_t *label_time_end = NULL;

// Data management
static lv_timer_t *data_timer = NULL;

// Navigation management
static lv_obj_t *image_straight = NULL;
static lv_obj_t *image_turn_left = NULL;
static lv_obj_t *image_turn_right = NULL;
static lv_obj_t *label_nav_text = NULL;

// Hint animation
static lv_obj_t *hint_trigger_objs[6] = {NULL};  // camera, image_2, phone, image_4, play, image_5

// Player state
static music_player_state_t player_state = {
    .song_duration_ms = 140000,
    .current_progress_ms = 0,
    .is_playing = 0,
    .current_song_idx = 0,
    .is_initialized = 0
};

/* ========== Function Forward Declarations ========== */

// Utility functions
static lv_obj_t* get_valid_obj(lv_obj_t *obj);
static void format_time_to_str(uint32_t time_ms, char *buf, uint8_t buf_size);

// Hint animation
static void show_developing_hint(void);
static void hint_pressed_cb(lv_event_t *e);
static void bind_hint_event(lv_obj_t *obj);
static void hint_animation_init(screen_main_t *scr);

// Data management
static const char* data_manager_get_value(void);
static void data_manager_next_value(void);
static uint8_t data_manager_get_index(void);
static void data_manager_init(void);
static void data_timer_cb(lv_timer_t *timer);

// Navigation management
static void nav_update_by_index(uint8_t idx);
static void get_nav_ui_objects(screen_main_t *scr);

// Music playback
static void music_timer_cb(lv_timer_t *timer);
static void update_progress_bar(void);
static void update_time_labels(void);
static void update_play_button_state(void);
static void reset_progress(void);
static void switch_album(uint8_t idx);
static void image_start_cb(lv_event_t *e);
static void image_stop_cb(lv_event_t *e);
static void image_forward_cb(lv_event_t *e);
static void image_back_cb(lv_event_t *e);
static void image_next_cb(lv_event_t *e);
static void image_pre_cb(lv_event_t *e);
static void pause_and_switch_screen(void);
static void screen_main_container_set_clicked(lv_event_t *e);
static void screen_main_image_set_clicked(lv_event_t *e);
static void get_ui_objects(screen_main_t *scr);
static void bind_button_events(screen_main_t *scr);

/* ========== Hint Animation Management ========== */

static void show_developing_hint(void)
{
    screen_main_t *scr = screen_main_get(&ui_manager);
    if (!scr || !scr->label_wait) {
        return;
    }
    
    lv_obj_t *label = scr->label_wait;
    if (!lv_obj_is_valid(label)) {
        return;
    }
    
    lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(label, (lv_anim_exec_xcb_t)lv_obj_set_y);
    
    lv_obj_set_pos(label, 384, 18);
    
    lv_anim_t anim_x;
    lv_anim_init(&anim_x);
    lv_anim_set_var(&anim_x, label);
    lv_anim_set_exec_cb(&anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&anim_x, 384, 384);
    lv_anim_set_time(&anim_x, 1500);
    lv_anim_set_repeat_count(&anim_x, 1);
    lv_anim_set_delay(&anim_x, 1000);
    lv_anim_set_path_cb(&anim_x, lv_anim_path_linear);
    lv_anim_start(&anim_x);

    lv_anim_t anim_y;
    lv_anim_init(&anim_y);
    lv_anim_set_var(&anim_y, label);
    lv_anim_set_exec_cb(&anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&anim_y, 18, -30);
    lv_anim_set_time(&anim_y, 1500);
    lv_anim_set_repeat_count(&anim_y, 1);
    lv_anim_set_delay(&anim_y, 1000);
    lv_anim_set_path_cb(&anim_y, lv_anim_path_linear);
    lv_anim_start(&anim_y);
}

static void hint_pressed_cb(lv_event_t *e)
{
    (void)e;
    show_developing_hint();
}

static void bind_hint_event(lv_obj_t *obj)
{
    if (!obj || !lv_obj_is_valid(obj)) {
        return;
    }
    lv_obj_add_event_cb(obj, hint_pressed_cb, LV_EVENT_PRESSED, NULL);
}

static void hint_animation_init(screen_main_t *scr)
{
    hint_trigger_objs[0] = scr->container_camera;
    hint_trigger_objs[1] = scr->image_camera;
    hint_trigger_objs[2] = scr->container_phone;
    hint_trigger_objs[3] = scr->image_phone;
    hint_trigger_objs[4] = scr->container_play;
    hint_trigger_objs[5] = scr->image_play;
    
    for (int i = 0; i < 6; i++) {
        bind_hint_event(hint_trigger_objs[i]);
    }
}

/* ========== Data Management ========== */

static const char* data_manager_get_value(void)
{
    return s_data_values[s_data_index];
}

static void data_manager_next_value(void)
{
    s_data_index = (s_data_index + 1) % s_data_value_count;
}

static uint8_t data_manager_get_index(void)
{
    return s_data_index;
}

static void data_manager_init(void)
{
    if (data_timer == NULL) {
        data_timer = lv_timer_create(data_timer_cb, 5000, NULL);
    }
    lv_timer_resume(data_timer);
}

static void data_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    
    screen_main_t *scr = screen_main_get(&ui_manager);
    if (!scr || !scr->obj || !lv_obj_is_valid(scr->obj)) {
        return;
    }
    
    if (lv_scr_act() != scr->obj) {
        return;
    }
    
    data_manager_next_value();
    const char *value = data_manager_get_value();
    uint8_t idx = data_manager_get_index();
    
    if (scr->label_meter && lv_obj_is_valid(scr->label_meter)) {
        lv_label_set_text(scr->label_meter, value);
    }
    
    nav_update_by_index(idx);
}

/* ========== Navigation Management ========== */

static void get_nav_ui_objects(screen_main_t *scr)
{
    image_straight  = get_valid_obj(scr->image_straight);
    image_turn_left = get_valid_obj(scr->image_turn_left);
    image_turn_right= get_valid_obj(scr->image_turn_right);
    label_nav_text  = get_valid_obj(scr->label_address);
}

static void nav_update_by_index(uint8_t idx)
{
    uint8_t nav_idx = idx % NAV_STATE_COUNT;
    
    if (image_straight)  lv_obj_add_flag(image_straight, LV_OBJ_FLAG_HIDDEN);
    if (image_turn_left) lv_obj_add_flag(image_turn_left, LV_OBJ_FLAG_HIDDEN);
    if (image_turn_right)lv_obj_add_flag(image_turn_right, LV_OBJ_FLAG_HIDDEN);
    
    switch (nav_idx) {
        case 0:
            if (image_straight) lv_obj_clear_flag(image_straight, LV_OBJ_FLAG_HIDDEN);
            break;
        case 1:
            if (image_turn_left) lv_obj_clear_flag(image_turn_left, LV_OBJ_FLAG_HIDDEN);
            break;
        case 2:
            if (image_turn_right) lv_obj_clear_flag(image_turn_right, LV_OBJ_FLAG_HIDDEN);
            break;
    }
    
    if (label_nav_text) {
        lv_label_set_text(label_nav_text, nav_texts[nav_idx]);
    }
}

/* ========== Music Playback Management ========== */

static lv_obj_t* get_valid_obj(lv_obj_t *obj)
{
    return (obj && lv_obj_is_valid(obj)) ? obj : NULL;
}

static void format_time_to_str(uint32_t time_ms, char *buf, uint8_t buf_size)
{
    if (buf_size < 6) {
        return;
    }
    
    uint32_t total_seconds = time_ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    
    lv_snprintf(buf, buf_size, "%lu:%02lu", minutes, seconds);
}

static void get_ui_objects(screen_main_t *scr)
{
    bar_progress     = get_valid_obj(scr->bar_progress);
    image_album      = get_valid_obj(scr->image_album1);
    image_start      = get_valid_obj(scr->image_start);
    image_stop       = get_valid_obj(scr->image_stop);
    image_forward    = get_valid_obj(scr->image_forward);
    image_back       = get_valid_obj(scr->image_back);
    image_next       = get_valid_obj(scr->image_next);
    image_pre        = get_valid_obj(scr->image_pre);
    label_time_start = get_valid_obj(scr->label_time_start);
    label_time_end   = get_valid_obj(scr->label_time_end);
}

static void update_progress_bar(void)
{
    if (!bar_progress || !lv_obj_is_valid(bar_progress)) {
        return;
    }
    
    uint8_t percentage = (uint8_t)((player_state.current_progress_ms * 100ULL) / player_state.song_duration_ms);
    lv_bar_set_value(bar_progress, percentage, LV_ANIM_OFF);
}

static void update_time_labels(void)
{
    char time_str[8];
    
    if (label_time_start && lv_obj_is_valid(label_time_start)) {
        format_time_to_str(player_state.current_progress_ms, time_str, sizeof(time_str));
        lv_label_set_text(label_time_start, time_str);
    }
    
    if (label_time_end && lv_obj_is_valid(label_time_end)) {
        format_time_to_str(player_state.song_duration_ms, time_str, sizeof(time_str));
        lv_label_set_text(label_time_end, time_str);
    }
}

static void update_play_button_state(void)
{
    if (!image_start || !image_stop) {
        return;
    }
    
    if (!lv_obj_is_valid(image_start) || !lv_obj_is_valid(image_stop)) {
        return;
    }
    
    if (player_state.is_playing) {
        lv_obj_add_flag(image_start, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(image_stop, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(image_stop, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(image_start, LV_OBJ_FLAG_HIDDEN);
    }
}

static void reset_progress(void)
{
    player_state.current_progress_ms = 0;
    player_state.is_playing = 0;
    update_progress_bar();
    update_time_labels();
    update_play_button_state();
}

static void switch_album(uint8_t idx)
{
    if (!image_album || !lv_obj_is_valid(image_album)) {
        return;
    }
    
    idx = idx % ALBUM_COUNT;
    player_state.current_song_idx = idx;
    lv_img_set_src(image_album, album_images[idx]);
}

static void pause_and_switch_screen(void)
{
    player_state.is_playing = 0;
    update_play_button_state();
    
    if (music_timer) {
        lv_timer_pause(music_timer);
    }
    if (data_timer) {
        lv_timer_pause(data_timer);
    }
    
    lv_obj_t *act_scr = lv_scr_act();
    if (!screen_is_loading(act_scr)) {
        lv_obj_clean(act_scr);
        screen_1_create(&ui_manager);
        lv_scr_load_anim(screen_1_get(&ui_manager)->obj, LV_SCR_LOAD_ANIM_NONE, 0, 0, ui_manager.auto_del);
    }
}

static void screen_main_container_set_clicked(lv_event_t *e)
{
    (void)e;
    pause_and_switch_screen();
}

static void screen_main_image_set_clicked(lv_event_t *e)
{
    (void)e;
    pause_and_switch_screen();
}

static void music_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    
    if (!player_state.is_playing) {
        return;
    }
    
    if (!bar_progress || !label_time_start) {
        player_state.is_playing = 0;
        return;
    }
    
    if (!lv_obj_is_valid(bar_progress) || !lv_obj_is_valid(label_time_start)) {
        player_state.is_playing = 0;
        return;
    }
    
    player_state.current_progress_ms += UPDATE_PERIOD_MS;
    
    if (player_state.current_progress_ms >= player_state.song_duration_ms) {
        player_state.current_progress_ms = player_state.song_duration_ms;
        player_state.is_playing = 0;
        update_play_button_state();
    }
    
    update_progress_bar();
    update_time_labels();
}

static void image_start_cb(lv_event_t *e)
{
    (void)e;
    if (!bar_progress || !lv_obj_is_valid(bar_progress)) {
        return;
    }
    
    player_state.is_playing = 1;
    update_play_button_state();
}

static void image_stop_cb(lv_event_t *e)
{
    (void)e;
    player_state.is_playing = 0;
    update_play_button_state();
}

static void image_forward_cb(lv_event_t *e)
{
    (void)e;
    
    uint32_t new_progress = player_state.current_progress_ms + SKIP_TIME_MS;
    player_state.current_progress_ms = (new_progress > player_state.song_duration_ms) 
                                        ? player_state.song_duration_ms 
                                        : new_progress;
    update_progress_bar();
    update_time_labels();
}

static void image_back_cb(lv_event_t *e)
{
    (void)e;
    
    player_state.current_progress_ms = (player_state.current_progress_ms <= SKIP_TIME_MS) 
                                        ? 0 
                                        : player_state.current_progress_ms - SKIP_TIME_MS;
    update_progress_bar();
    update_time_labels();
}

static void image_next_cb(lv_event_t *e)
{
    (void)e;
    reset_progress();
    switch_album(player_state.current_song_idx + 1);
}

static void image_pre_cb(lv_event_t *e)
{
    (void)e;
    reset_progress();
    switch_album((player_state.current_song_idx + ALBUM_COUNT - 1) % ALBUM_COUNT);
}

static void bind_button_events(screen_main_t *scr)
{
    if (image_start)   lv_obj_add_event_cb(image_start, image_start_cb, LV_EVENT_CLICKED, NULL);
    if (image_stop)    lv_obj_add_event_cb(image_stop, image_stop_cb, LV_EVENT_CLICKED, NULL);
    if (image_forward) lv_obj_add_event_cb(image_forward, image_forward_cb, LV_EVENT_CLICKED, NULL);
    if (image_back)    lv_obj_add_event_cb(image_back, image_back_cb, LV_EVENT_CLICKED, NULL);
    if (image_next)    lv_obj_add_event_cb(image_next, image_next_cb, LV_EVENT_CLICKED, NULL);
    if (image_pre)     lv_obj_add_event_cb(image_pre, image_pre_cb, LV_EVENT_CLICKED, NULL);
    
    if (scr->container_set) {
        lv_obj_add_event_cb(scr->container_set, screen_main_container_set_clicked, LV_EVENT_CLICKED, NULL);
    }
    if (scr->image_set) {
        lv_obj_add_event_cb(scr->image_set, screen_main_image_set_clicked, LV_EVENT_CLICKED, NULL);
    }
}

/* ========== Initialization and Cleanup ========== */

void screen_main_custom_init(void)
{
    screen_main_t *scr = screen_main_get(&ui_manager);
    if (!scr) {
        return;
    }
    
    get_ui_objects(scr);
    if (!bar_progress || !image_album) {
        return;
    }
    
    lv_bar_set_range(bar_progress, 0, 100);
    update_progress_bar();
    switch_album(player_state.current_song_idx);
    update_time_labels();
    bind_button_events(scr);
    
    if (music_timer == NULL) {
        music_timer = lv_timer_create(music_timer_cb, UPDATE_PERIOD_MS, NULL);
    } else {
        lv_timer_resume(music_timer);
    }
    
    player_state.is_playing = 0;
    update_play_button_state();
    player_state.is_initialized = 1;
    
    data_manager_init();
    
    if (scr->label_meter && lv_obj_is_valid(scr->label_meter)) {
        lv_label_set_text(scr->label_meter, data_manager_get_value());
    }
    
    get_nav_ui_objects(scr);
    nav_update_by_index(s_data_index);
    hint_animation_init(scr);
}

void screen_main_custom_cleanup(void)
{
    player_state.is_playing = 0;
    
    if (music_timer) {
        lv_timer_pause(music_timer);
    }
    if (data_timer) {
        lv_timer_pause(data_timer);
    }
    
    screen_main_t *scr = screen_main_get(&ui_manager);
    if (scr && scr->label_wait && lv_obj_is_valid(scr->label_wait)) {
        lv_anim_del(scr->label_wait, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_del(scr->label_wait, (lv_anim_exec_xcb_t)lv_obj_set_y);
    }
    
    bar_progress     = NULL;
    image_album      = NULL;
    image_start      = NULL;
    image_stop       = NULL;
    image_forward    = NULL;
    image_back       = NULL;
    image_next       = NULL;
    image_pre        = NULL;
    label_time_start = NULL;
    label_time_end   = NULL;
    
    player_state.is_initialized = 0;
    
    image_straight  = NULL;
    image_turn_left = NULL;
    image_turn_right= NULL;
    label_nav_text  = NULL;
    
    for (int i = 0; i < 6; i++) {
        hint_trigger_objs[i] = NULL;
    }
}

/* ========== External Interface ========== */

void screen_main_music_set_duration(uint32_t duration_ms)
{
    if (duration_ms == 0) {
        return;
    }
    
    player_state.song_duration_ms = duration_ms;
    
    if (player_state.current_progress_ms > duration_ms) {
        player_state.current_progress_ms = duration_ms;
        if (player_state.is_initialized) {
            update_progress_bar();
            update_time_labels();
        }
    }
}

uint8_t screen_main_music_is_playing(void)
{
    return player_state.is_playing;
}

uint32_t screen_main_music_get_progress(void)
{
    return player_state.current_progress_ms;
}

void screen_main_music_set_progress(uint32_t progress_ms)
{
    if (progress_ms > player_state.song_duration_ms) {
        progress_ms = player_state.song_duration_ms;
    }
    
    player_state.current_progress_ms = progress_ms;
    
    if (player_state.is_initialized) {
        update_progress_bar();
        update_time_labels();
    }
}
