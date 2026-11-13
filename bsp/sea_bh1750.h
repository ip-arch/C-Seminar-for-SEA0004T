// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_bh1750.h -- 照度センサ BH1750 (I2C アドレス 0x5C) */
#ifndef SEA_BH1750_H
#define SEA_BH1750_H
#include <stdint.h>
uint8_t  bh1750_init(void);       /* 成功なら 1 */
uint16_t bh1750_read_lux(void);   /* 直近の測定値 (lx)。測定は約 120ms ごとに更新される */
#endif
