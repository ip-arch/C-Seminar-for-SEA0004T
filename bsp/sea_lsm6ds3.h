// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_lsm6ds3.h -- 加速度センサ LSM6DS3TR (I2C アドレス 0x6B) */
#ifndef SEA_LSM6DS3_H
#define SEA_LSM6DS3_H
#include <stdint.h>

uint8_t lsm6ds3_init(void);                 /* 成功なら 1 */
/* 加速度 (X, Y, Z) を 0.01 m/s^2 単位で返す。成功なら 0 */
uint8_t lsm6ds3_read_accel(int32_t accel_cms2[3]);
#endif
