/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#loader-with-arc

#include "lvgl.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "example";

static lv_obj_t * btn;
static lv_display_rotation_t rotation = LV_DISP_ROTATION_0;
extern TaskHandle_t ota_task_handle;

lv_obj_t* label;

static void btn_cb(lv_event_t * e)
{
    ESP_LOGI(TAG,"Button Clicked");
    lv_obj_t *scr = lv_display_get_screen_active(NULL);
    // Get the second child ie label
    lv_obj_t *child = lv_obj_get_child(scr,1);
    if(child){
        if(lv_obj_check_type(child,&lv_label_class)){
            lv_label_set_text(child,"Updating...");
            lv_obj_align(child,LV_ALIGN_CENTER,0,0);
            ESP_LOGI(TAG,"Found child: %p",child);
        }
    }
    // Notify ota_task to let it run
    xTaskNotifyGive(ota_task_handle);
}
static void set_angle(void * obj, int32_t v)
{
    lv_arc_set_value(obj, v);
}

void example_lvgl_demo_ui(lv_display_t *disp)
{
    lv_obj_t *scr = lv_display_get_screen_active(disp);

    btn = lv_button_create(scr);
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text_static(lbl, LV_SYMBOL_REFRESH" UPDATE");
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 30, -30);
    /*Button event*/
    lv_obj_add_event_cb(btn, btn_cb, LV_EVENT_CLICKED, disp);

    label = lv_label_create(scr);
    lv_label_set_text(label,"Hello World!");
    lv_obj_align(label,LV_ALIGN_CENTER,0,0);

    /*Create an Arc*/
    // lv_obj_t * arc = lv_arc_create(scr);
    // lv_arc_set_rotation(arc, 270);
    // lv_arc_set_bg_angles(arc, 0, 360);
    // lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    // lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    // lv_obj_center(arc);

    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_var(&a, arc);
    // lv_anim_set_exec_cb(&a, set_angle);
    // lv_anim_set_duration(&a, 1000);
    // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);    /*Just for the demo*/
    // lv_anim_set_repeat_delay(&a, 500);
    // lv_anim_set_values(&a, 0, 100);
    // lv_anim_start(&a);
}
