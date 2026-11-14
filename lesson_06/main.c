// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_06: 暗くなったら LED を点ける街灯 (Elecrow L_6 Intelligent_street_light) */

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
    sea_led_init();

    for (;;) {
        uint16_t lux = bh1750_read_lux();
        printf("[-] Light: [%u] lx\n", lux);

        /* 100 lx 以下なら暗いので点灯 */
        sea_led_write(lux <= 100);

        delay_ms(120);          /* 照度センサの測定が更新される間隔 */
    }
}
