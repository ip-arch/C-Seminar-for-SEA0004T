// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_17: 人感センサで LED を点ける (Elecrow L_17 PIR_control_light) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"

int main(void)
{
    uint8_t old_state = 0;                 /* 前回の人感センサの状態 */

    uart_init();
    sea_pir_init();
    sea_led_init();

    for (;;) {
        uint8_t state = sea_pir_detected();

        if (state && old_state != state) {            /* 動きを検知した */
            puts("[-] Motion detected!");
            old_state = state;
            sea_led_on();
            delay_ms(5000);                           /* 5 秒間点灯 */
        } else if (!state && old_state != state) {    /* 動きがなくなった */
            puts("[-] No Motion!");
            sea_led_off();
            old_state = state;
        }
        delay_ms(20);
    }
}
