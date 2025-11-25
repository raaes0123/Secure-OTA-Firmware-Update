#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lcd.h"
#include "esp_wifi.h"


#define WIFI_SSID "WIFISSID"         // SET THIS!
#define WIFI_PASS "WIFIPassword"        // SET THIS!

// LVGL library is not thread-safe, this example will call LVGL APIs from different tasks, so use a mutex to protect it
_lock_t lvgl_api_lock;
// create a lvgl display
lv_display_t *display;

extern void wifi_init(const char *ssid, const char *pass);
extern void ota_update_task(void);
extern void example_lvgl_demo_ui(lv_disp_t *disp);
extern void example_lvgl_port_task(void *arg);

static const char *TAG = "example";
TaskHandle_t ota_task_handle = NULL;

// Queue to send update-progress from ota_task to lvgl_task
QueueHandle_t lvgl_queue;

void app_main(void)
{
    // Disconnect previous connection
    esp_wifi_disconnect();
    // Setup wifi. This function is implemented in wifi.c
    // It blocks until connected to the configured WiFi network
    wifi_init(WIFI_SSID, WIFI_PASS);
    printf("Connected to WIFI\n");

    lcd_config(display);
    
    lvgl_queue = xQueueCreate(10, sizeof(int));  // 10 messages, each an int

    ESP_LOGI(TAG, "Create LVGL task in Core 1");
    xTaskCreatePinnedToCore(example_lvgl_port_task, "LVGL", 4096, NULL, 2, NULL,1);
    ESP_LOGI(TAG, "Create OTA task in Core 0");
    xTaskCreatePinnedToCore((TaskFunction_t)ota_update_task,"ota_task",20*1024,NULL,2,&ota_task_handle,0);
    ESP_LOGI(TAG, "Display LVGL Meter Widget");
    // Lock the mutex since LVGL APIs are not thread-safe
    _lock_acquire(&lvgl_api_lock);
    example_lvgl_demo_ui(display);
    _lock_release(&lvgl_api_lock);
}





