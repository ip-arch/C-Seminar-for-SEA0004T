// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_8: 障害物が近づいたらリレーと LED を動かす (Elecrow L_8 Obstacle_close_range_alarm) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_hcsr04.h"

int main(void)
{
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
    sea_relay_init();
    sea_led_init();

    lcd_print_lines("Distance    30", "");

    for (;;) {
        int16_t distance = hcsr04_cm();
        if (distance < 0) {
            continue;
        }

        uint32_t now = time_millis();
        if (now - previous >= 500) {
            previous = now;

            if (distance >= 30) {                 /* 30cm 以上: 遠い */
                lcd_print_at(9, 0, ">=");
                sea_relay_write(0);
                sea_led_off();
            } else {                              /* 30cm 未満: 近い */
                lcd_print_at(9, 0, "< ");
                sea_relay_write(1);
                sea_led_on();
            }
            printf("%d\n", distance);
        }
        delay_ms(100);
    }
}
