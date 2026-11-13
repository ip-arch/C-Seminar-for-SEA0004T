// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_pwm.h"
#include <avr/io.h>

void pwm_init(void)
{
    DDRB  |= (1 << PB2);
    /* 高速 PWM 8bit、1/64 分周 (約 976Hz) */
    TCCR1A = (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1B  = 0;
}

void pwm_write(uint8_t duty)
{
    if (duty == 0) {                       /* 完全に消灯 */
        TCCR1A &= ~(1 << COM1B1);
        PORTB  &= ~(1 << PB2);
    } else if (duty == 255) {              /* 完全に点灯 */
        TCCR1A &= ~(1 << COM1B1);
        PORTB  |=  (1 << PB2);
    } else {
        OCR1B   = duty;
        TCCR1A |= (1 << COM1B1);
    }
}
