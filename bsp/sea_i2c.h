// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_i2c.h -- I2C (A4=SDA, A5=SCL) のマスタ。100kHz
 *   どの関数も、成功なら 0、失敗なら 0 以外を返す */
#ifndef SEA_I2C_H
#define SEA_I2C_H
#include <stdint.h>

void    i2c_init(void);

/* addr のデバイスへ len バイト書く */
uint8_t i2c_write(uint8_t addr, const uint8_t *data, uint8_t len);

/* addr のデバイスから len バイト読む */
uint8_t i2c_read(uint8_t addr, uint8_t *data, uint8_t len);

/* 書いてから (リスタートで) 読む。レジスタ読み出し用 */
uint8_t i2c_write_read(uint8_t addr, const uint8_t *wdata, uint8_t wlen,
                       uint8_t *rdata, uint8_t rlen);
#endif
