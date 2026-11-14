// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_07: 超音波センサで距離を測って表示する (Elecrow L_7 Ultrasonic_ranging_display) */

#include <stdio.h>
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_hcsr04.h"

int main(void)
{
    char buf[8];
    uint32_t previous = 0;

    uart_init();
    time_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");
    hcsr04_init();

    lcd_print_lines("Distance:", "");

    for (;;) {
        int16_t distance = hcsr04_cm();
        if (distance < 0) {
            continue;                         /* 測れなかったのでやり直し */
        }

        /* 500ms ごとに表示を更新 */
        uint32_t now = time_millis();
        if (now - previous >= 500) {
            previous = now;
            snprintf(buf, sizeof buf, "%d", distance);
            lcd_print_at(0, 1, "   ");
            lcd_print_at(0, 1, buf);
        }
        delay_ms(10);
    }
}
