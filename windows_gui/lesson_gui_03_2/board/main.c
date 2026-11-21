// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* comm_1 (ボード側): PC からの命令で LED を操作する
 *   PC → ボード  "LED ON" / "LED OFF"
 *   ボード → PC  "OK" / "ERROR"
 * 起動したとき、"READY" を 1 行送る */

#include <stdio.h>
#include <string.h>
#include "sea0004t.h"
#include "sea_uart.h"
#include "../protocol.h"

int main(void)
{
    char line[32];

    uart_init();
    sea_led_init();
    puts(PROTO_READY);

    for (;;) {
        if (fgets(line, sizeof line, stdin) == NULL) {
            continue;
        }
        line[strcspn(line, "\r\n")] = '\0';      /* 行末の改行を取る */

        if (strcmp(line, CMD_LED_ON) == 0) {
            sea_led_on();
            puts(RES_OK);
        } else if (strcmp(line, CMD_LED_OFF) == 0) {
            sea_led_off();
            puts(RES_OK);
        } else {
            puts(RES_ERROR);
        }
    }
}
