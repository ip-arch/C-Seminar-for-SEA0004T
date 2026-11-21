// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_03 入力ボックスとメッセージボックスを表示する */

#include <stdio.h>
#include "gui_app.h"
#include "win_msgbox.h"

/* #define INPUTBOX GUI_APP_TEXT_INPUT */
#define INPUTBOX 0

static int click_count = 0;

void on_button_click(void)
{
    char buf[100];
    click_count++;
    snprintf(buf, sizeof(buf), "ボタンが押されました（%d回目）", click_count);
    message_box("ボタン", buf);
}

int main(void)
{
    run_gui_app(INPUTBOX,"イベント駆動サンプル", "クリック","表示");
    return 0;
}
