// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_03_2 ボタンによりLEDをON/OFFする */

#include <stdio.h>
#include "uart_helper.h"
#include "../protocol.h"
#include "gui_app.h"
#include "win_msgbox.h"

/* #define INPUTBOX GUI_APP_TEXT_INPUT */
#define INPUTBOX 0

static int click_count = 0;
UartStream uart;

void on_button_click(void)
{

    static int led = 0;
    char buf[100], line[64];
    click_count++;
    if (!led) {
        fprintf(uart.out, "%s\n", CMD_LED_ON);       /* 送る (fflush は不要) */
        if (uart_fgets(line, sizeof line, &uart) != NULL) {
            sprintf(buf,"%-8s -> 応答: %s", CMD_LED_ON, line);
            message_box("応答", buf);
        }
    }
    else {
        fprintf(uart.out, "%s\n", CMD_LED_OFF);
        if (uart_fgets(line, sizeof line, &uart) != NULL) {
            sprintf(buf,"%-8s -> 応答: %s", CMD_LED_OFF, line);
            message_box("応答", buf);
        }
    }
    led = !led;
}


int main(void)
{
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
    run_gui_app(INPUTBOX, "LED制御", "クリック", "表示");

    close_uart_stream(&uart);
    return 0;
}
