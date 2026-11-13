// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_hcsr04.h"
#include "sea_time.h"
#include <avr/io.h>
#include <util/delay.h>

#define TRIG_BIT   PD6
#define ECHO_BIT   PD5
#define TIMEOUT_US 30000UL     /* 約 5m 相当 */

void hcsr04_init(void)
{
    DDRD  |=  (1 << TRIG_BIT);
    PORTD &= ~(1 << TRIG_BIT);
    DDRD  &= ~(1 << ECHO_BIT);
}

int16_t hcsr04_cm(void)
{
    /* TRIG に 10us のパルスを出す */
    PORTD &= ~(1 << TRIG_BIT);
    _delay_us(2);
    PORTD |=  (1 << TRIG_BIT);
    _delay_us(10);
    PORTD &= ~(1 << TRIG_BIT);

    /* ECHO が HIGH になるのを待つ */
    uint32_t t0 = time_micros();
    while (!(PIND & (1 << ECHO_BIT))) {
        if (time_micros() - t0 > TIMEOUT_US) return -1;
    }

    /* HIGH の長さを測る */
    uint32_t start = time_micros();
    while (PIND & (1 << ECHO_BIT)) {
        if (time_micros() - start > TIMEOUT_US) return -1;
    }
    uint32_t width = time_micros() - start;

    return (int16_t)(width / 58UL);        /* 往復 58us ごとに 1cm */
}
