// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_pwm.h -- LED (D10) の明るさ調整。Timer1 を使う
 *   注意: サーボ(sea_servo)と Timer1 を共用するので、同時には使えない */
#ifndef SEA_PWM_H
#define SEA_PWM_H
#include <stdint.h>
void pwm_init(void);
void pwm_write(uint8_t duty);   /* 0 (消灯) 〜 255 (最大の明るさ) */
#endif
