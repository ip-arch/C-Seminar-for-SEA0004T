// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_04_1 入力ボックスとメッセージをLCDに表示する */

#include <stdio.h>
#include "gui_app.h"
#include "win_msgbox.h"
#include "uart_helper.h"
#include "../protocol.h"

#define INPUTBOX GUI_APP_TEXT_INPUT
/* #define INPUTBOX 0 */
static UartStream g_uart;   /* ボタンの処理から使うので、関数の外に置く */

void on_button_click(void)
{
    char buf[256];
    get_input_text(buf, sizeof(buf));
    fprintf(g_uart.out, "%15s\n", buf);
    message_box("入力された文字列", buf);
}

int main(void)
{
    console_utf8();

    if (!open_uart_stream_sea0004t(&g_uart, 115200)) {
        printf("エラー: ボードを開けません (%s)\n", uart_last_error());
        return 1;
    }
    if (!uart_wait_line(&g_uart, PROTO_READY, 5000)) {
        printf("エラー: ボードから %s が届きません\n", PROTO_READY);
        close_uart_stream(&g_uart);
        return 1;
    }


    
    run_gui_app(INPUTBOX,"LCD駆動サンプル", "クリック","表示",NULL,0);
    return 0;
}
