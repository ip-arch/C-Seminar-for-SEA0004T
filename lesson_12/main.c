// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_12: サーボモータを 0〜180 度に往復させる (Elecrow L_12 Servo_control) */

#include "sea_time.h"
#include "sea_servo.h"

int main(void)
{
    servo_attach();

    for (;;) {
        for (int pos = 0; pos <= 180; pos++) {    /* 0 度 → 180 度 */
            servo_write((uint8_t)pos);
            delay_ms(15);
        }
        delay_ms(2000);

        for (int pos = 180; pos >= 0; pos--) {    /* 180 度 → 0 度 */
            servo_write((uint8_t)pos);
            delay_ms(15);
        }
        delay_ms(2000);
    }
}
