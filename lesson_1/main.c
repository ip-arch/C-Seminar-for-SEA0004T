// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_1: LED を 1 秒ごとに点滅させる (Elecrow L_1 LED_control) */

#include "sea0004t.h"
#include "sea_time.h"

int main(void)
{
    sea_led_init();

    for (;;) {
        sea_led_on();
        delay_ms(1000);
        sea_led_off();
        delay_ms(1000);
    }
}
