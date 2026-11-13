// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_hcsr04.h -- 超音波距離センサ HC-SR04 (TRIG=D6, ECHO=D5)
 *   hcsr04_cm() を使う前に time_init() を呼ぶこと */
#ifndef SEA_HCSR04_H
#define SEA_HCSR04_H
#include <stdint.h>
void    hcsr04_init(void);
int16_t hcsr04_cm(void);   /* 距離 (cm)。測れなかったときは -1 */
#endif
