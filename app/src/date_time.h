#ifndef _DATE_TIME_H
#define _DATE_TIME_H
#include <time.h>
#include <counter.h>

#define RTC_ALARM_PERIOD_MS 1000

#define SEC_TO_MS    1000
#define HOUR_TO_SECS 3600
#define HOUR_TO_MINS 60
#define DAY_TO_HOURS 24
#define DAY_TO_SECS (DAY_TO_HOURS * HOUR_TO_SECS)

typedef struct {
    struct device *rtc_counter_dev;
    struct counter_alarm_cfg rtc_config;
    struct tm time_civil_tm;
    time_t time_epoch_s;
    struct k_mutex mutex;
} date_time_module;

enum months {
    JAN = 0,
    FEB,
    MAR,
    APR,
    MAY,
    JUN,
    JUL,
    AUG,
    SEP,
    OCT,
    NOV,
    DEC,
    MONTHS_NUM
};

enum wkdays {
    SUN = 0,
    MON,
    TUE,
    WED,
    THU,
    FRI,
    SAT,
    WKDAYS_NUM
};

extern const char *months_str[MONTHS_NUM];
extern const char *wkdays_str[WKDAYS_NUM];

void date_time_init(date_time_module *date_time_mod);
void rtc_alarm_cb(struct device *dev, u8_t chan_id, u32_t ticks, void *user_data);
int rtc_counter_dev_init(date_time_module *date_time_mod);

#endif /* _DATE_TIME_H */