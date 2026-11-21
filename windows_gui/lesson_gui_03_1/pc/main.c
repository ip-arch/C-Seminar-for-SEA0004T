// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_03_2 入力ボックスとメッセージボックスを表示する */

#include <stdio.h>
#include "gui_app.h"
#include "win_msgbox.h"

#define INPUTBOX GUI_APP_TEXT_INPUT
/* #define INPUTBOX 0 */


void on_button_click(void)
{
    char buf[256];
    get_input_text(buf, sizeof(buf));
    message_box("入力された文字列", buf);
}

int main(void)
{
    run_gui_app(INPUTBOX,"イベント駆動サンプル", "クリック","表示",NULL,0);
    return 0;
}
