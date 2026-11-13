// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_adc.h -- アナログ入力 (10bit: 0〜1023) */
#ifndef SEA_ADC_H
#define SEA_ADC_H
#include <stdint.h>
void     adc_init(void);
uint16_t adc_read(uint8_t channel);   /* channel は 0〜7 (A0〜A7) */
#endif
