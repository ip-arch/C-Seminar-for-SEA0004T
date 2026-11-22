// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* センサー選択サンプル (ボード側)
 *   PC → ボード  "GET TEMP" など
 *   ボード → PC  "TEMP 243" / "ACCEL 12 -5 979" / "ERROR"
 * 名前の表は、protocol.h の SENSOR_LIST から作る。
 * 日本語の表示名と単位は、ここには取り込まない (ボードの RAM は 2KB しかないため)。 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sea0004t.h"
#include "sea_uart.h"
#include "sea_adc.h"
#include "sea_i2c.h"
#include "sea_dht20.h"
#include "sea_bh1750.h"
#include "sea_lsm6ds3.h"
#include "../protocol.h"

 /* 通信での名前と、値の個数だけを、表にする */
static const char* const kTokens[SENSOR_COUNT] = {
#define X(id, tok, ja, n, dec, unit, min, max)  tok,
    SENSOR_LIST(X)
#undef X
};
static const uint8_t kCount[SENSOR_COUNT] = {
#define X(id, tok, ja, n, dec, unit, min, max)  n,
    SENSOR_LIST(X)
#undef X
};

/* センサーごとの読み取り。値は v[0]、v[1]、... に入れる。成功したら 1 */
static int read_sensor(int id, int32_t v[])
{
    switch (id) {
    case SENSOR_TEMP: {
        int16_t temp, humi;                         /* 10 倍した整数 */
        if (dht20_read(&temp, &humi) != 0) return 0;
        v[0] = temp;
        return 1;
    }
    case SENSOR_LUX:
        v[0] = bh1750_read_lux();
        return 1;
    case SENSOR_ACCEL:                              /* 100 倍した整数 (X、Y、Z) */
        return lsm6ds3_read_accel(v) == 0;
    case SENSOR_POT:
        v[0] = adc_read(ADC_POT);
        return 1;
    case SENSOR_SOUND:
        v[0] = adc_read(ADC_SOUND);
        return 1;
    default:
        return 0;
    }
}

/* 名前から、番号を探す。見つからなければ -1 */
static int find_sensor(const char* name)
{
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (strcmp(name, kTokens[i]) == 0) return i;
    }
    return -1;
}

int main(void)
{
    char line[32];
    int32_t v[SENSOR_MAX_VALUES];

    uart_init();
    adc_init();
    i2c_init();
    dht20_init();                    /* 失敗しても進む。読むときに ERROR を返す */
    bh1750_init();
    lsm6ds3_init();
    puts(PROTO_READY);

    for (;;) {
        if (fgets(line, sizeof line, stdin) == NULL) {
            continue;
        }
        line[strcspn(line, "\r\n")] = '\0';

        /* "GET " で始まっているか */
        size_t head = strlen(CMD_GET) + 1;
        if (strncmp(line, CMD_GET " ", head) != 0) {
            puts(RES_ERROR);
            continue;
        }

        int id = find_sensor(line + head);
        if (id < 0 || !read_sensor(id, v)) {
            puts(RES_ERROR);
            continue;
        }

        printf("%s", kTokens[id]);                   /* 例: TEMP */
        for (uint8_t i = 0; i < kCount[id]; i++) {
            printf(" %ld", (long)v[i]);              /* 例: " 243" */
        }
        putchar('\n');
    }
}