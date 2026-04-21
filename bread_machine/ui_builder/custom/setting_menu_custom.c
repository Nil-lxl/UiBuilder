#include "custom.h"

static lang_type_t current_lang = AIC_LANG_EN;

void bread_setting_menu_custom_created(void) 
{
    bread_setting_menu_t *scr = bread_setting_menu_get(&ui_manager);
    switch(current_lang) {
        case 0:
        {
            lv_dropdown_set_selected(scr->lan_switch, 0);
            break;
        }
        case 1:
        {
            lv_dropdown_set_selected(scr->lan_switch, 2);
            break;
        }
        case 2:
        {
            lv_dropdown_set_selected(scr->lan_switch, 1);
            break;
        }
        default:
            break;
        }
}

void bread_setting_menu_eng_switch_custom_value_changed(void) 
{
    bread_setting_menu_update_language(AIC_LANG_EN);
    current_lang = AIC_LANG_EN;
    
    bread_setting_menu_t *scr = bread_setting_menu_get(&ui_manager);
    lv_dropdown_set_selected(scr->lan_switch, 0);
}

void bread_setting_menu_chn_switch_custom_value_changed(void) 
{
    bread_setting_menu_update_language(AIC_LANG_ZH);
    current_lang = AIC_LANG_ZH;

    bread_setting_menu_t *scr = bread_setting_menu_get(&ui_manager);
    lv_dropdown_set_selected(scr->lan_switch, 1);
}

void bread_setting_menu_jpn_switch_custom_value_changed(void) 
{
    bread_setting_menu_update_language(AIC_LANG_JP);
    current_lang = AIC_LANG_JP;

    bread_setting_menu_t *scr = bread_setting_menu_get(&ui_manager);
    lv_dropdown_set_selected(scr->lan_switch, 2);
}
