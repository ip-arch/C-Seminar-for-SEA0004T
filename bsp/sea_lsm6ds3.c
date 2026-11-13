// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_lsm6ds3.h"
#include "sea_i2c.h"

#define LSM_ADDR      0x6B
#define CTRL1_XL      0x10
#define CTRL2_G       0x11
#define OUTX_L_XL     0x28

static uint8_t write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    return i2c_write(LSM_ADDR, buf, 2);
}

uint8_t lsm6ds3_init(void)
{
    uint8_t err = write_reg(CTRL1_XL, 0x48);   /* 加速度: 104Hz、±4g */
    err |= write_reg(CTRL2_G, 0x40);           /* ジャイロ: 104Hz、250dps */
    return err == 0;
}

uint8_t lsm6ds3_read_accel(int32_t accel_cms2[3])
{
    uint8_t reg = OUTX_L_XL;
    uint8_t d[6];
    if (i2c_write_read(LSM_ADDR, &reg, 1, d, 6) != 0) {
        return 1;
    }
    for (uint8_t i = 0; i < 3; i++) {
        int16_t raw = (int16_t)((uint16_t)d[i * 2] | ((uint16_t)d[i * 2 + 1] << 8));
        /* ±4g では 1 LSB = 0.000122 g。1g = 9.80 m/s^2 とすると
         * raw * 0.000122 * 9.80 * 100 = raw * 0.11956 [0.01 m/s^2] */
        accel_cms2[i] = (int32_t)raw * 1196L / 10000L;
    }
    return 0;
}
