#include <drivers/counter.h>
#include <sys/timeutil.h>
#include <string.h>
#include "date_time.h"

const char *months_str[MONTHS_NUM] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

const char *wkdays_str[WKDAYS_NUM] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};

date_time_module curr_date_time = {0};

void date_time_init(date_time_module *date_time_mod)
{
    k_mutex_init(&date_time_mod->mutex);

    /* Set Mon 1, 2020 at 12:00 */
    date_time_mod->time_civil_tm.tm_sec = 57;
    date_time_mod->time_civil_tm.tm_min = 59;
    date_time_mod->time_civil_tm.tm_hour = 23;
    date_time_mod->time_civil_tm.tm_mday = 1;
    date_time_mod->time_civil_tm.tm_mon = JAN;
    date_time_mod->time_civil_tm.tm_year = 2021 - 1900;
    date_time_mod->time_civil_tm.tm_wday = FRI;
    date_time_mod->time_civil_tm.tm_yday = 1;
    date_time_mod->time_civil_tm.tm_isdst = -1;

    /* Convert civil time to epoch time */
    date_time_mod->time_epoch_s = timeutil_timegm(&curr_date_time.time_civil_tm);
}

void rtc_alarm_cb(struct device *dev, u8_t chan_id, u32_t ticks, void *user_data)
{
    struct tm *civil_time = NULL;

    k_mutex_lock(&curr_date_time.mutex, K_MSEC(100));
    ++curr_date_time.time_epoch_s;
    /* Convert epoch time to civil */
    civil_time = gmtime(&curr_date_time.time_epoch_s);
    memcpy(&curr_date_time.time_civil_tm, civil_time, sizeof(struct tm));
    k_mutex_unlock(&curr_date_time.mutex);

    counter_set_channel_alarm(dev, 0, &curr_date_time.rtc_config);
}

int rtc_counter_dev_init(date_time_module *date_time_mod)
{
    date_time_mod->rtc_counter_dev = device_get_binding(DT_NORDIC_NRF_RTC_RTC_2_LABEL);
    if (date_time_mod->rtc_counter_dev == NULL) 
        return -EINVAL;

    counter_start(date_time_mod->rtc_counter_dev);

    date_time_mod->rtc_config.flags = 0;
    date_time_mod->rtc_config.ticks = counter_us_to_ticks(date_time_mod->rtc_counter_dev, RTC_ALARM_PERIOD_MS * USEC_PER_MSEC);
    date_time_mod->rtc_config.callback = rtc_alarm_cb;
    date_time_mod->rtc_config.user_data = NULL;

    counter_set_channel_alarm(date_time_mod->rtc_counter_dev, 0, &date_time_mod->rtc_config);

    return 0;
}