// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_18: 音を検知したらブザーを鳴らす (Elecrow L_18 Sound_Reminder) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_adc.h"
#include "sea_tone.h"

#define SOUND_THRESHOLD  400

int main(void)
{
    uart_init();
    adc_init();

    for (;;) {
        if (adc_read(ADC_SOUND) >= SOUND_THRESHOLD) {
            puts("[-] Detect Sound!");
            tone_start(1300);
            delay_ms(1000);

            /* 音が小さくなるまで鳴らし続ける */
            for (;;) {
                uint16_t value = adc_read(ADC_SOUND);
                if (value >= SOUND_THRESHOLD) {
                    printf("%u\n", value);
                } else {
                    tone_stop();
                    break;
                }
            }
        }
    }
}
