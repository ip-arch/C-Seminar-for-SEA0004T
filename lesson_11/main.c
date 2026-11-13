// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_11: 温度と湿度を測って表示する (Elecrow L_11 Temperature_Humidity_detecting_system) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_dht20.h"

int main(void)
{
    char tbuf[12], hbuf[12], line[16];
    uint8_t count = 0;
    uint32_t last_read = 0;

    uart_init();
    time_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");
    if (!dht20_init()) {
        puts("DHT20 not found");
    }

    lcd_print_lines("Temp:", "Humi:");

    for (;;) {
        if (time_millis() - last_read >= 1000) {
            int16_t temp, humi;
            last_read = time_millis();

            if (dht20_read(&temp, &humi) != 0) {
                puts("DHT20 read error");
                continue;
            }

            if ((count % 10) == 0) {              /* 10 回ごとに見出しを出す */
                count = 0;
                puts("");
                puts("Type\tHumidity (%)\tTemp (C)");
            }
            count++;

            sea_fixed_str(tbuf, sizeof tbuf, temp, 1);
            sea_fixed_str(hbuf, sizeof hbuf, humi, 1);
            printf("DHT20 \t%s\t%s\n", hbuf, tbuf);

            snprintf(line, sizeof line, "%s C    ", tbuf);
            lcd_print_at(5, 0, line);
            snprintf(line, sizeof line, "%s %%    ", hbuf);
            lcd_print_at(5, 1, line);
        }
        delay_ms(10);
    }
}
