#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <zephyr.h>
#include <drivers/gpio.h>

#define LED_PORT        DT_ALIAS_LED1_GPIOS_CONTROLLER
#define LED             DT_ALIAS_LED1_GPIOS_PIN

static void backlight_init(void)
{
    struct device *dev;

    dev = device_get_binding(LED_PORT);

    gpio_pin_configure(dev, LED, GPIO_DIR_OUT);
    gpio_pin_write(dev, LED, 0);
}

int main(void)
{
    struct device *display_dev;

    display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
    if (display_dev == NULL)
        return -1;
    else
        backlight_init();

    static lv_style_t main_screen_st;
    lv_style_copy(&main_screen_st, &lv_style_plain);
    main_screen_st.body.main_color = LV_COLOR_BLACK;
    main_screen_st.body.grad_color = LV_COLOR_BLACK;

    lv_obj_set_style(lv_scr_act(), &main_screen_st);

    lv_obj_t *time_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(time_label, LV_LABEL_ALIGN_CENTER);
    int hours = 12;
    int minutes = 30;
    lv_label_set_text_fmt(time_label, "%d : %d", hours, minutes);

    static lv_style_t time_label_style;
    lv_style_copy(&time_label_style, &lv_style_plain);
    time_label_style.text.color = LV_COLOR_MAGENTA;
    time_label_style.text.font = &lv_font_roboto_28;

    lv_obj_set_style(time_label, &time_label_style);

    lv_obj_set_size(time_label, 80, 70);
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t *date_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(date_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(date_label, " Mon 12 2020");

    static lv_style_t date_label_style;
    lv_style_copy(&date_label_style, &lv_style_plain);
    date_label_style.text.color = LV_COLOR_GREEN;
    date_label_style.text.font = &lv_font_roboto_28;

    lv_obj_set_style(date_label, &date_label_style);

    lv_obj_set_size(date_label, 80, 70);
    lv_obj_align(date_label, time_label, LV_ALIGN_OUT_BOTTOM_MID, 0 , 0);

    display_blanking_off(display_dev);

    while (1) {
        lv_task_handler();
        k_sleep(K_MSEC(10));
    }

    return 0;

}
