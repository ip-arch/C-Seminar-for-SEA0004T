// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_i2c.h"
#include <avr/io.h>

#define TW_START         0x08
#define TW_REP_START     0x10
#define TW_MT_SLA_ACK    0x18
#define TW_MT_DATA_ACK   0x28
#define TW_MR_SLA_ACK    0x40

void i2c_init(void)
{
    PORTC |= (1 << PC4) | (1 << PC5);          /* 内蔵プルアップ */
    TWSR = 0;                                  /* 分周なし */
    TWBR = (uint8_t)((F_CPU / 100000UL - 16UL) / 2UL);   /* 100kHz */
    TWCR = (1 << TWEN);
}

/* TWINT が立つのを待つ。待ちすぎたら 0 */
static uint8_t wait_twint(void)
{
    uint16_t guard = 0xFFFF;
    while (!(TWCR & (1 << TWINT))) {
        if (--guard == 0) return 0;
    }
    return 1;
}

static uint8_t status(void) { return TWSR & 0xF8; }

static void stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    uint16_t guard = 0xFFFF;
    while ((TWCR & (1 << TWSTO)) && --guard) { }
}

/* スタート (またはリスタート) とアドレス送信。read=1 で読み出し */
static uint8_t start(uint8_t addr, uint8_t read, uint8_t restart)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (!wait_twint()) return 1;
    uint8_t s = status();
    if (s != (restart ? TW_REP_START : TW_START)) return 2;

    TWDR = (uint8_t)((addr << 1) | (read ? 1 : 0));
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!wait_twint()) return 3;
    s = status();
    if (s != (read ? TW_MR_SLA_ACK : TW_MT_SLA_ACK)) return 4;   /* NACK */
    return 0;
}

static uint8_t send_bytes(const uint8_t *data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        TWDR = data[i];
        TWCR = (1 << TWINT) | (1 << TWEN);
        if (!wait_twint()) return 1;
        if (status() != TW_MT_DATA_ACK) return 2;
    }
    return 0;
}

static uint8_t recv_bytes(uint8_t *data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        uint8_t last = (i == len - 1);
        TWCR = (1 << TWINT) | (1 << TWEN) | (last ? 0 : (1 << TWEA));
        if (!wait_twint()) return 1;
        data[i] = TWDR;
    }
    return 0;
}

uint8_t i2c_write(uint8_t addr, const uint8_t *data, uint8_t len)
{
    uint8_t err = start(addr, 0, 0);
    if (!err) err = send_bytes(data, len);
    stop();
    return err;
}

uint8_t i2c_read(uint8_t addr, uint8_t *data, uint8_t len)
{
    uint8_t err = start(addr, 1, 0);
    if (!err) err = recv_bytes(data, len);
    stop();
    return err;
}

uint8_t i2c_write_read(uint8_t addr, const uint8_t *wdata, uint8_t wlen,
                       uint8_t *rdata, uint8_t rlen)
{
    uint8_t err = start(addr, 0, 0);
    if (!err) err = send_bytes(wdata, wlen);
    if (!err) err = start(addr, 1, 1);
    if (!err) err = recv_bytes(rdata, rlen);
    stop();
    return err;
}
