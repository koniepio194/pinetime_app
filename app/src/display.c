#include <kernel.h>
#include <drivers/gpio.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <sys/util.h>
#include "display.h"
#include "date_time.h"

#define DATE_BUF_LEN 30

struct device *disp_dev = NULL;
struct device *backlight_dev = NULL;
struct k_mutex lvgl_mutex;
lv_obj_t *lvgl_main_screen_objs[OBJS_NUM] = {NULL};
struct k_timer lvgl_tick_timer;
extern date_time_module curr_date_time;

/* Thread for periodical lv_task_handler() call */
void lvgl_task_handler_run(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    while (1) {
        k_mutex_lock(&lvgl_mutex, K_FOREVER);
        lv_task_handler();
        k_mutex_unlock(&lvgl_mutex);
        k_sleep(5);
    }
}

/* Thread for displaying time on a main screen */
void main_screen_time(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    date_time_module date_time_temp;

    while (1) {
        k_mutex_lock(&curr_date_time.mutex, K_FOREVER);
        memcpy(&date_time_temp, &curr_date_time, sizeof(date_time_module));
        k_mutex_unlock(&curr_date_time.mutex);

        k_mutex_lock(&lvgl_mutex, K_FOREVER);
        lv_label_set_text_fmt(lvgl_main_screen_objs[TIME_LABEL], "%02d : %02d : %02d", date_time_temp.time_civil_tm.tm_hour,
                                                                                       date_time_temp.time_civil_tm.tm_min,
                                                                                       date_time_temp.time_civil_tm.tm_sec);
        k_mutex_unlock(&lvgl_mutex);
        k_sleep(200);
    }
}

/* Thread for displaying date on a main screen */
void main_screen_date(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    date_time_module date_time_temp;
    int current_wday = -1, next_date_change_secs;
    char date_buf[DATE_BUF_LEN] = {0};

    while (1) {
        k_mutex_lock(&curr_date_time.mutex, K_FOREVER);
        memcpy(&date_time_temp, &curr_date_time, sizeof(date_time_module));
        k_mutex_unlock(&curr_date_time.mutex);

        k_mutex_lock(&lvgl_mutex, K_FOREVER);
        if (current_wday != date_time_temp.time_civil_tm.tm_wday) {
            tm_to_date_str(&date_time_temp.time_civil_tm, date_buf, DATE_BUF_LEN);
            lv_label_set_text_fmt(lvgl_main_screen_objs[DATE_LABEL], "%s\n", date_buf);
        }
        /* Compute how many secs left to next date change */
        next_date_change_secs = DAY_TO_SECS - (date_time_temp.time_civil_tm.tm_hour * HOUR_TO_SECS) -
                                (date_time_temp.time_civil_tm.tm_min * HOUR_TO_MINS) - date_time_temp.time_civil_tm.tm_sec;
        current_wday = date_time_temp.time_civil_tm.tm_wday;
        k_mutex_unlock(&lvgl_mutex);
        /* Put this thread into sleep until next plannig date change */
        k_sleep(next_date_change_secs * SEC_TO_MS);
    }
}

int display_backlight_init(struct device **dev)
{
    *dev = device_get_binding(LED_PORT);
    if (*dev == NULL)
        return -EINVAL;

    gpio_pin_configure(*dev, LED, GPIO_DIR_OUT);
    gpio_pin_write(*dev, LED, 0);

    return 0;
}

int display_dev_init(struct device **dev)
{
    *dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
    if (*dev == NULL)
        return -EINVAL;
    else
        return 0;
}

void lvgl_screens_create(date_time_module *date_time_mod)
{
    k_mutex_init(&lvgl_mutex);

    static lv_style_t main_screen_st;
    lv_style_copy(&main_screen_st, &lv_style_plain);
    main_screen_st.body.main_color = LV_COLOR_BLACK;
    main_screen_st.body.grad_color = LV_COLOR_BLACK;

    lv_obj_t *main_scr = lv_scr_act();
    lvgl_main_screen_objs[MAIN_SCR] = main_scr;
    lv_obj_set_style(main_scr, &main_screen_st);

    /* TIME label */
    lv_obj_t *time_label = lv_label_create(lv_scr_act(), NULL);
    lvgl_main_screen_objs[TIME_LABEL] = time_label;
    lv_label_set_align(time_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text_fmt(time_label, "%02d : %02d : %02d", date_time_mod->time_civil_tm.tm_hour,
                                                            date_time_mod->time_civil_tm.tm_min,
                                                            date_time_mod->time_civil_tm.tm_sec);

    static lv_style_t time_label_style;
    lv_style_copy(&time_label_style, &lv_style_plain);
    time_label_style.text.color = LV_COLOR_MAGENTA;
    time_label_style.text.font = &lv_font_roboto_28;

    lv_obj_set_style(time_label, &time_label_style);

    lv_obj_set_size(time_label, 80, 70);
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -20);

    /* DATE label */
    lv_obj_t *date_label = lv_label_create(lv_scr_act(), NULL);
    lvgl_main_screen_objs[DATE_LABEL] = date_label;
    lv_label_set_align(date_label, LV_LABEL_ALIGN_CENTER);
    char date_buf[30] = {0};
    tm_to_date_str(&date_time_mod->time_civil_tm, date_buf, 30);
    lv_label_set_text_fmt(date_label, "%s", date_buf);

    static lv_style_t date_label_style;
    lv_style_copy(&date_label_style, &lv_style_plain);
    date_label_style.text.color = LV_COLOR_GREEN;
    date_label_style.text.font = &lv_font_roboto_28;

    lv_obj_set_style(date_label, &date_label_style);

    lv_obj_set_size(date_label, 80, 70);
    lv_obj_align(date_label, time_label, LV_ALIGN_OUT_BOTTOM_MID, 0 , 0);
}

void lvgl_tick_timer_expiry(struct k_timer *timer_id)
{
    ARG_UNUSED(timer_id);

    lv_tick_inc(LV_TICK_INC_PERIOD_MS);
}

char *tm_to_date_str(struct tm *time_date, char *time_buf, int len)
{
    char temp[5] = {0};

    strncpy(time_buf, wkdays_str[time_date->tm_wday], len);
    strncat(time_buf, ", ", len);
    u8_to_dec(temp, sizeof(temp), (u8_t)time_date->tm_mday);
    strncat(time_buf, temp, len);
    strncat(time_buf, " ", len);
    strncat(time_buf, months_str[time_date->tm_mon], len);

    return time_buf;
}