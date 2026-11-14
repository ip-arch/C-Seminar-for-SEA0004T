// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_03: 可変抵抗で LED の明るさを変える (Elecrow L_3 Breathing_LED) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_adc.h"
#include "sea_pwm.h"

int main(void)
{
    uart_init();
    adc_init();
    pwm_init();
    puts("Breathing LED start");

    for (;;) {
        uint16_t adc = adc_read(ADC_POT);                    /* 0〜1023 */
        uint8_t duty = (uint8_t)sea_map(adc, 0, 1023, 0, 255); /* 0〜255 */
        pwm_write(duty);
        printf("adc=%u duty=%u\n", adc, duty);
        delay_ms(100);
    }
}
