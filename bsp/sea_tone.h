// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_tone.h -- ブザー (D3) を鳴らす。Timer2 を使う */
#ifndef SEA_TONE_H
#define SEA_TONE_H
#include <stdint.h>
void tone_start(uint16_t freq_hz);   /* 約 31Hz〜 の音を鳴らし続ける */
void tone_stop(void);
#endif
