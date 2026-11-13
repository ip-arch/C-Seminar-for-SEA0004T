// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_dht20.h -- 温湿度センサ DHT20 (I2C アドレス 0x38)
 *   値は 10 倍した整数で扱う (例: 24.3℃ → 243) */
#ifndef SEA_DHT20_H
#define SEA_DHT20_H
#include <stdint.h>

uint8_t dht20_init(void);                              /* 成功なら 1 */
/* 測定して値を返す。成功なら 0、失敗なら 0 以外。測定は 1 秒に 1 回まで */
uint8_t dht20_read(int16_t *temp_x10, int16_t *humi_x10);
#endif
