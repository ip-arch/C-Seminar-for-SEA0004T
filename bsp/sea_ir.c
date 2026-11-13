// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_ir.h"
#include "sea_time.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * NEC 方式の受信 (INT0 の立ち下がりごとに割り込み)
 *   リーダ   : 9ms の搬送波 + 4.5ms の無信号 → 立ち下がり間隔 約 13.5ms
 *   ビット 0 : 立ち下がり間隔 約 1.125ms
 *   ビット 1 : 立ち下がり間隔 約 2.25ms
 *   32 ビットを下位ビットから受け取る (アドレス, ~アドレス, コマンド, ~コマンド)
 *   受信モジュールの出力は、搬送波があると LOW になる
 */
static volatile uint32_t last_us;
static volatile uint32_t shift;
static volatile uint8_t  nbits;
static volatile uint8_t  receiving;
static volatile uint8_t  ready;
static volatile uint8_t  command;

ISR(INT0_vect)
{
    uint32_t now = time_micros();
    uint32_t dt  = now - last_us;
    last_us = now;

    if (dt > 12000UL && dt < 15000UL) {            /* リーダ */
        receiving = 1;
        nbits = 0;
        shift = 0;
        return;
    }
    if (!receiving) {
        return;                                    /* リピート信号などは無視 */
    }

    if (dt >= 900UL && dt <= 1500UL) {
        /* ビット 0: 何もしない */
    } else if (dt >= 1800UL && dt <= 2700UL) {
        shift |= (1UL << nbits);                   /* ビット 1 */
    } else {
        receiving = 0;                             /* 間隔がおかしい */
        return;
    }

    if (++nbits == 32) {
        receiving = 0;
        command = (uint8_t)(shift >> 16);          /* 3 バイト目がコマンド */
        ready = 1;
    }
}

void ir_init(void)
{
    DDRD  &= ~(1 << PD2);
    EICRA  = (1 << ISC01);                         /* 立ち下がりエッジ */
    EIFR   = (1 << INTF0);
    EIMSK |= (1 << INT0);
    sei();
}

uint8_t ir_available(void)
{
    return ready;
}

uint8_t ir_read(void)
{
    uint8_t c = command;
    ready = 0;
    return c;
}
