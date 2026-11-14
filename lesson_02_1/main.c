// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_2_1: ボタンで LED を切り替える (Elecrow L_2 Button_Control_LED) */

#include "sea0004t.h"
#include "sea_time.h"
#define DEBOUNCE 10

uint8_t key_value(void);

int main(void)
{
    uint8_t led_val = 0, prev = 0;
    sea_led_init();
    sea_button_init();

    for (;;) {
        uint8_t cur = sea_button_pressed(); /* key_value() との差を比較 */
        /* ボタンを押す瞬間に LED が反転する */
        if (prev == 0 && cur == 1) {
            led_val = ! led_val;
            sea_led_write(led_val);
        }
        prev = cur;
    }
}

uint8_t key_value(void) {
    uint8_t prev, cur = sea_button_pressed();
    do {
        prev = cur;
        delay_ms(DEBOUNCE);
        cur = sea_button_pressed();
    } while (prev != cur);
    return cur;
}

