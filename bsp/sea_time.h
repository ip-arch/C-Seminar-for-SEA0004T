// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_time.h -- 時間まわり (Timer0 を 1ms ごとに使う) */
#ifndef SEA_TIME_H
#define SEA_TIME_H
#include <stdint.h>

void     time_init(void);       /* millis/micros を使う前に 1 回呼ぶ */
uint32_t time_millis(void);     /* 起動してからのミリ秒 */
uint32_t time_micros(void);     /* 起動してからのマイクロ秒 (4us 単位) */
void     delay_ms(uint16_t ms); /* ms ミリ秒待つ (time_init 不要) */

#endif
