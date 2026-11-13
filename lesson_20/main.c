// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_20: 廊下の照明 (Elecrow L_20 Smart_corridor_light)
 *   暗い (100 lx 未満) とき、人感センサか音センサが反応したら LED を 10 秒点ける */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_bh1750.h"

int main(void)
{
    uart_init();
    i2c_init();
    if (bh1750_init()) {
        puts("BH1750 Advanced begin");
    } else {
        puts("Error initialising BH1750");
    }
    sea_pir_init();
    sea_sound_digital_init();
    sea_led_init();

    for (;;) {
        int lux = (int)bh1750_read_lux();
        printf("[-] Light: [%d] lx\n", lux);

        if (lux < 100) {
            /* 暗いあいだ、人と音を監視し続ける */
            for (;;) {
                uint8_t pir = sea_pir_detected();
                uint8_t snd = sea_sound_digital();

                if (pir || snd) {
                    puts("[-] Motion detected!");
                    sea_led_on();
                    delay_ms(10000);
                } else {
                    puts("[-] No Motion!");
                    sea_led_off();
                }

                lux = (int)bh1750_read_lux();
                if (lux >= 100) break;             /* 明るくなったら抜ける */
            }
        } else {
            sea_led_off();
        }
        delay_ms(100);
    }
}
