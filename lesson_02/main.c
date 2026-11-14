// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_02: ボタンで LED を点ける (Elecrow L_2 Button_Control_LED) */

#include "sea0004t.h"
#include "sea_time.h"

int main(void)
{
    sea_led_init();
    sea_button_init();

    for (;;) {
        /* ボタンを押している間だけ LED が点く */
        sea_led_write(sea_button_pressed());
        delay_ms(100);
    }
}
