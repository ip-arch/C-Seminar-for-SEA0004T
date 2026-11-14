// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_09: 水やりのお知らせ (Elecrow L_9 Plant_Watering_Reminder_System)
 *   土壌水分が少ないほど、LED とブザーで知らせる */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_adc.h"
#include "sea_tone.h"
#include "sea_i2c.h"
#include "sea_lcd.h"

int main(void)
{
    char buf[8];

    uart_init();
    adc_init();
    sea_led_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");

    lcd_print_lines("Soil moisture", "");

    for (;;) {
        uint16_t value = adc_read(ADC_MOISTURE);
        printf("sensor1 = %u\n", value);

        int percent = (int)sea_map(value, 0, 1023, 0, 100);
        snprintf(buf, sizeof buf, "%d%%", percent);
        lcd_print_at(0, 1, "    ");
        lcd_print_at(0, 1, buf);
        delay_ms(500);

        if (percent < 10) {                       /* とても乾いている */
            sea_led_off();
            tone_start(1300);
            delay_ms(250);
            tone_stop();
        } else if (percent < 20) {                /* 乾いてきた */
            sea_led_on();
            delay_ms(100);
            sea_led_off();
        } else {                                  /* 十分に湿っている */
            sea_led_on();
        }
    }
}
