#define CONFIG_EXAMPLE_LCD_CONTROLLER_ILI9341 1
#define CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_XPT2046 1
#define CONFIG_EXAMPLE_LCD_TOUCH_ENABLED 1


//2.8-inch color TFT display screen with ILI9341 driver chip
//Display resolution: 240x320px with resistive touchscreen


#define LCD_HOST  SPI2_HOST
#define TOUCH_HOST  SPI3_HOST
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_SCLK           14
#define EXAMPLE_PIN_NUM_MOSI           13
#define EXAMPLE_PIN_NUM_MISO           12
#define EXAMPLE_PIN_NUM_LCD_DC         2
#define EXAMPLE_PIN_NUM_LCD_RST        -1
#define EXAMPLE_PIN_NUM_LCD_CS         15
#define EXAMPLE_PIN_NUM_BK_LIGHT       21
#define EXAMPLE_PIN_NUM_TOUCH_CS       33

////////////////// Touch-Screen /////////////////////////////////////
#define TOUCH_CS     33
#define TOUCH_CLK    25
#define TOUCH_MOSI   32
#define TOUCH_MISO   39

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              240
#define EXAMPLE_LCD_V_RES              320
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

#define EXAMPLE_LVGL_DRAW_BUF_LINES    20 // number of display lines in each draw buffer
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2

void lcd_config();
