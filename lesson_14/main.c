// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_14: 気温と湿度のお知らせ (Elecrow L_14 Weather_reminder)
 *   25℃超で LED 点灯、30℃超で点滅。湿度 40% 未満でブザー */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_dht20.h"
#include "sea_tone.h"

int main(void)
{
    char tbuf[12], hbuf[12], line1[17], line2[17];
    uint8_t count = 0;
    uint8_t led_state = 1;
    uint32_t last_read = 0;

    uart_init();
    time_init();
    sea_led_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");
    if (!dht20_init()) {
        puts("DHT20 not found");
    }

    for (;;) {
        if (time_millis() - last_read >= 1000) {
            int16_t temp, humi;
            last_read = time_millis();

            if (dht20_read(&temp, &humi) != 0) {
                puts("DHT20 read error");
                continue;
            }

            if ((count % 10) == 0) {
                count = 0;
                puts("");
                puts("Type\tHumidity (%)\tTemp (C)");
            }
            count++;

            sea_fixed_str(tbuf, sizeof tbuf, temp, 1);
            sea_fixed_str(hbuf, sizeof hbuf, humi, 1);
            printf("DHT20 \t%s\t%s\n", hbuf, tbuf);

            int temp_int = temp / 10;             /* 小数点以下は切り捨て */
            int humi_int = humi / 10;
            printf("%d\n%d\n", humi_int, temp_int);

            if (temp_int > 25 && temp_int <= 30) {
                lcd_print_lines("High Temperature", " ");
                sea_led_on();
            } else if (temp_int > 30) {
                lcd_print_lines("Hot Temperature", " ");
                led_state = !led_state;           /* 1 秒ごとに点滅 */
                sea_led_write(led_state);
            } else {
                snprintf(line1, sizeof line1, "Tem:%sC", tbuf);
                snprintf(line2, sizeof line2, "Hum:%s%%", hbuf);
                lcd_print_lines(line1, line2);
                sea_led_off();
            }

            if (humi < 400) {                     /* 湿度 40% 未満 */
                lcd_print_at(0, 1, "Dry Air");
                tone_start(1300);
            } else {
                tone_stop();
            }
        }
        delay_ms(10);
    }
}
