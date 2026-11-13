// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
﻿/* comm_2 (PC 側): ボードの可変抵抗の値を、0.5 秒ごとに 10 回読んで表示する */

#include <stdio.h>
#include "uart_helper.h"
#include "../protocol.h"

int main(void)
{
    UartStream uart;
    char line[64];

    console_utf8();

    if (!open_uart_stream_sea0004t(&uart, 115200)) {
        printf("エラー: ボードを開けません (%s)\n", uart_last_error());
        return 1;
    }
    if (!uart_wait_line(&uart, PROTO_READY, 5000)) {
        printf("エラー: ボードから %s が届きません\n", PROTO_READY);
        close_uart_stream(&uart);
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        int value = 0;

        fprintf(uart.out, "%s\n", CMD_GET_POT);      /* 聞く */
        if (uart_fgets(line, sizeof line, &uart) == NULL) {
            printf("エラー: 応答がありません\n");
            break;
        }
        if (sscanf(line, RES_POT " %d", &value) == 1) {   /* "POT 355" から 355 を取り出す */
            printf("可変抵抗 = %4d\n", value);
        } else {
            printf("想定外の応答: %s", line);
        }
        uart_sleep_ms(500);
    }

    close_uart_stream(&uart);
    return 0;
}
