// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_time.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile uint32_t ms_count;

/* Timer0: 16MHz / 64 = 250kHz。250 カウントで 1ms */
ISR(TIMER0_COMPA_vect)
{
    ms_count++;
}

void time_init(void)
{
    TCCR0A = (1 << WGM01);                    /* CTC モード */
    OCR0A  = 249;
    TCCR0B = (1 << CS01) | (1 << CS00);       /* 1/64 */
    TIMSK0 = (1 << OCIE0A);
    sei();
}

uint32_t time_millis(void)
{
    uint8_t sreg = SREG;
    cli();
    uint32_t m = ms_count;
    SREG = sreg;
    return m;
}

uint32_t time_micros(void)
{
    uint8_t sreg = SREG;
    cli();
    uint32_t m = ms_count;
    uint8_t  t = TCNT0;
    if ((TIFR0 & (1 << OCF0A)) && t < 249) {  /* 割り込み待ちの 1ms 分を足す */
        m++;
    }
    SREG = sreg;
    return m * 1000UL + (uint32_t)t * 4UL;
}

void delay_ms(uint16_t ms)
{
    while (ms--) {
        _delay_ms(1);
    }
}
