// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_servo.h -- サーボモータ (D9)。Timer1 を使う
 *   注意: LED の PWM (sea_pwm) と Timer1 を共用するので、同時には使えない */
#ifndef SEA_SERVO_H
#define SEA_SERVO_H
#include <stdint.h>
void    servo_attach(void);            /* パルスを出し始める */
void    servo_detach(void);            /* パルスを止める (力が抜ける) */
void    servo_write(uint8_t degrees);  /* 0〜180 度 */
uint8_t servo_read(void);              /* 最後に指示した角度 */
#endif
