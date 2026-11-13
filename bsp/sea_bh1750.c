// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_bh1750.h"
#include "sea_i2c.h"
#include "sea_time.h"

#define BH1750_ADDR           0x5C
#define BH1750_CONT_HRES      0x10     /* 連続・高分解能モード (1 lx) */
#define BH1750_MTREG_DEFAULT  69

static uint8_t send_cmd(uint8_t cmd)
{
    return i2c_write(BH1750_ADDR, &cmd, 1);
}

uint8_t bh1750_init(void)
{
    uint8_t err = send_cmd(BH1750_CONT_HRES);
    delay_ms(10);
    /* 測定時間レジスタを標準値 (69) に設定 */
    err |= send_cmd((uint8_t)(0x40 | (BH1750_MTREG_DEFAULT >> 5)));
    err |= send_cmd((uint8_t)(0x60 | (BH1750_MTREG_DEFAULT & 0x1F)));
    err |= send_cmd(BH1750_CONT_HRES);
    delay_ms(180);                     /* 最初の測定が終わるまで待つ */
    return err == 0;
}

uint16_t bh1750_read_lux(void)
{
    uint8_t d[2];
    if (i2c_read(BH1750_ADDR, d, 2) != 0) {
        return 0;
    }
    uint32_t raw = ((uint32_t)d[0] << 8) | d[1];
    return (uint16_t)(raw * 10UL / 12UL);        /* raw / 1.2 */
}
