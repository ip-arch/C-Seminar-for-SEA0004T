// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_02 入力ボックスとメッセージボックスを表示する */

#include <stdio.h>
#include <stdlib.h>

#include "win_msgbox.h"
#include "input_box.h"
int main(void)
{
    char buf[100];
    int a, b;
    input_box("1つ目の整数を入力してください", buf, sizeof(buf));
    a = atoi(buf);
    input_box("2つ目の整数を入力してください", buf, sizeof(buf));
    b = atoi(buf);
    char result[100];
    sprintf(result, "%d + %d = %d", a, b, a + b);
    message_box("計算結果", result);
    return 0;
}
