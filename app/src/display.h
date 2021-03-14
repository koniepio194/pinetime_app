#ifndef _DISPLAY_H
#define _DISPLAY_H
#include "date_time.h"

#define LED_PORT DT_ALIAS_LED1_GPIOS_CONTROLLER
#define LED      DT_ALIAS_LED1_GPIOS_PIN

#define LV_TICK_INC_PERIOD_MS 10

enum lvgl_main_screen_objs {
    MAIN_SCR = 0,
    TIME_LABEL,
    DATE_LABEL,
    OBJS_NUM
};

extern struct device *disp_dev;
extern struct device *backlight_dev;
extern struct k_mutex lvgl_mutex;
extern lv_obj_t *lvgl_main_screen_objs[OBJS_NUM];
extern struct k_timer lvgl_tick_timer;

void lvgl_task_handler_run(void *arg1, void *arg2, void *arg3);
void main_screen_time(void *arg1, void *arg2, void *arg3);
void main_screen_date(void *arg1, void *arg2, void *arg3);
int display_backlight_init(struct device **dev);
int display_dev_init(struct device **dev);
void lvgl_screens_create(date_time_module *date_time_mod);
void lvgl_tick_timer_expiry(struct k_timer *timer_id);
char *tm_to_date_str(struct tm *time_date, char *time_buf, int len);

#endif /* _DISPLAY_H */