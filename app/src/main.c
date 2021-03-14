#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <zephyr.h>
#include <drivers/gpio.h>
#include "display.h"
#include "date_time.h"

#define STACK_SIZE_LVGL 1024
#define LVGL_PRIORITY 5

K_THREAD_STACK_DEFINE(lvgl_stack, STACK_SIZE_LVGL);
struct k_thread thread_lvgl_data;

#define STACK_SIZE_MAIN_SCREEN_TIME 1024
#define MAIN_SCREEN_TIME_PRIORITY 5

K_THREAD_STACK_DEFINE(main_screen_time_stack, STACK_SIZE_MAIN_SCREEN_TIME);
struct k_thread thread_main_screen_time;

#define STACK_SIZE_MAIN_SCREEN_DATE 1024
#define MAIN_SCREEN_DATE_PRIORITY 5

K_THREAD_STACK_DEFINE(main_screen_date_stack, STACK_SIZE_MAIN_SCREEN_DATE);
struct k_thread thread_main_screen_date;

extern date_time_module curr_date_time;

void main(void)
{
    int rc = 0;

    rc = display_dev_init(&disp_dev);
    __ASSERT_NO_MSG(rc == 0);

    rc = display_backlight_init(&backlight_dev);
    __ASSERT_NO_MSG(rc == 0);

    date_time_init(&curr_date_time);
    rtc_counter_dev_init(&curr_date_time);

    /* Timer for LVGL tick - required for the internal timing of LittlevGL */
    k_timer_init(&lvgl_tick_timer, lvgl_tick_timer_expiry, NULL);
    /* Call lv_tick_inc() every 10ms */
    k_timer_start(&lvgl_tick_timer, K_MSEC(LV_TICK_INC_PERIOD_MS), K_MSEC(LV_TICK_INC_PERIOD_MS));

    lvgl_screens_create(&curr_date_time);
    display_blanking_off(disp_dev);

    k_thread_create(&thread_lvgl_data, lvgl_stack, K_THREAD_STACK_SIZEOF(lvgl_stack),
            lvgl_task_handler_run, NULL, NULL, NULL, LVGL_PRIORITY, 0, K_NO_WAIT);
    k_thread_create(&thread_main_screen_time, main_screen_time_stack, K_THREAD_STACK_SIZEOF(main_screen_time_stack),
            main_screen_time, NULL, NULL, NULL, MAIN_SCREEN_TIME_PRIORITY, 0, K_NO_WAIT);
    k_thread_create(&thread_main_screen_date, main_screen_date_stack, K_THREAD_STACK_SIZEOF(main_screen_date_stack),
            main_screen_date, NULL, NULL, NULL, MAIN_SCREEN_DATE_PRIORITY, 0, K_NO_WAIT);
}