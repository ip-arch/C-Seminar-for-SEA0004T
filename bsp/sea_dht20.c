// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_dht20.h"
#include "sea_i2c.h"
#include "sea_time.h"

#define DHT20_ADDR  0x38

static uint8_t read_status(uint8_t *st)
{
    return i2c_read(DHT20_ADDR, st, 1);
}

/* Aosong 社のデモコードに基づくレジスタ初期化 */
static uint8_t reset_register(uint8_t reg)
{
    uint8_t w[3] = { reg, 0x00, 0x00 };
    uint8_t v[3] = { 0, 0, 0 };
    if (i2c_write(DHT20_ADDR, w, 3) != 0) return 1;
    delay_ms(5);
    i2c_read(DHT20_ADDR, v, 3);
    delay_ms(10);
    w[0] = (uint8_t)(0xB0 | reg);
    w[1] = v[1];
    w[2] = v[2];
    if (i2c_write(DHT20_ADDR, w, 3) != 0) return 1;
    delay_ms(5);
    return 0;
}

static void reset_if_needed(void)
{
    uint8_t st = 0;
    read_status(&st);
    if ((st & 0x18) != 0x18) {
        reset_register(0x1B);
        reset_register(0x1C);
        reset_register(0x1E);
        delay_ms(10);
    }
}

static uint8_t crc8(const uint8_t *p, uint8_t len)
{
    uint8_t crc = 0xFF;
    while (len--) {
        crc ^= *p++;
        for (uint8_t i = 0; i < 8; i++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

uint8_t dht20_init(void)
{
    uint8_t st;
    delay_ms(100);                         /* 電源投入後の待ち */
    if (read_status(&st) != 0) return 0;
    reset_if_needed();
    return 1;
}

uint8_t dht20_read(int16_t *temp_x10, int16_t *humi_x10)
{
    uint8_t cmd[3] = { 0xAC, 0x33, 0x00 };
    uint8_t b[7];
    uint8_t st = 0;

    reset_if_needed();
    if (i2c_write(DHT20_ADDR, cmd, 3) != 0) return 1;

    delay_ms(80);                          /* 測定時間 */
    for (uint8_t i = 0; i < 100; i++) {    /* 測定中ビットが下りるまで待つ */
        if (read_status(&st) == 0 && !(st & 0x80)) break;
        delay_ms(1);
    }

    if (i2c_read(DHT20_ADDR, b, 7) != 0) return 2;
    if ((b[0] | b[1] | b[2] | b[3] | b[4] | b[5] | b[6]) == 0) return 3;
    if (crc8(b, 6) != b[6]) return 4;

    uint32_t raw_h = ((uint32_t)b[1] << 12) | ((uint32_t)b[2] << 4) | (b[3] >> 4);
    uint32_t raw_t = ((uint32_t)(b[3] & 0x0F) << 16) | ((uint32_t)b[4] << 8) | b[5];

    *humi_x10 = (int16_t)((raw_h * 1000UL) >> 20);               /* raw / 2^20 * 100 % */
    *temp_x10 = (int16_t)(((raw_t * 2000UL) >> 20) - 500);       /* raw / 2^20 * 200 - 50 ℃ */
    return 0;
}
