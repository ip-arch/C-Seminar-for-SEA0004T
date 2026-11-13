// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* comm_1 (PC 側): ボードの LED を、PC から 3 回点滅させる */

#include <stdio.h>
#include "uart_helper.h"
#include "../protocol.h"

int main(void)
{
    UartStream uart;
    char line[64];

    console_utf8();                                  /* 日本語を表示するための設定 */

    if (!open_uart_stream_sea0004t(&uart, 115200)) {
        printf("エラー: ボードを開けません (%s)\n", uart_last_error());
        return 1;
    }

    /* ポートを開くとボードが再起動するので、READY が届くのを待つ */
    printf("ボードの起動を待っています...\n");
    if (!uart_wait_line(&uart, PROTO_READY, 5000)) {
        printf("エラー: ボードから %s が届きません\n", PROTO_READY);
        close_uart_stream(&uart);
        return 1;
    }
    printf("接続しました\n");

    for (int i = 0; i < 3; i++) {
        fprintf(uart.out, "%s\n", CMD_LED_ON);       /* 送る (fflush は不要) */
        if (uart_fgets(line, sizeof line, &uart) != NULL) {
            printf("%-8s -> 応答: %s", CMD_LED_ON, line);
        }
        uart_sleep_ms(500);

        fprintf(uart.out, "%s\n", CMD_LED_OFF);
        if (uart_fgets(line, sizeof line, &uart) != NULL) {
            printf("%-8s -> 応答: %s", CMD_LED_OFF, line);
        }
        uart_sleep_ms(500);
    }

    close_uart_stream(&uart);
    return 0;
}
