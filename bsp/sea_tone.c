// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_tone.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* 半周期ごとに D3 を反転する */
ISR(TIMER2_COMPA_vect)
{
    PIND = (1 << PD3);
}

void tone_start(uint16_t freq_hz)
{
    static const uint16_t divider[] = { 64, 256, 1024 };
    static const uint8_t  csbits[]  = {
        (1 << CS22),
        (1 << CS22) | (1 << CS21),
        (1 << CS22) | (1 << CS21) | (1 << CS20)
    };

    if (freq_hz == 0) {
        tone_stop();
        return;
    }

    uint32_t ticks = F_CPU / (2UL * freq_hz);        /* 半周期あたりのクロック数 */
    for (uint8_t i = 0; i < 3; i++) {
        uint32_t o = ticks / divider[i];
        if (o >= 1 && o <= 256) {
            DDRD  |= (1 << PD3);
            TCCR2A = (1 << WGM21);                   /* CTC モード */
            OCR2A  = (uint8_t)(o - 1);
            TCNT2  = 0;
            TIMSK2 = (1 << OCIE2A);
            TCCR2B = csbits[i];
            sei();
            return;
        }
    }
}

void tone_stop(void)
{
    TIMSK2 = 0;
    TCCR2B = 0;
    PORTD &= ~(1 << PD3);
}
