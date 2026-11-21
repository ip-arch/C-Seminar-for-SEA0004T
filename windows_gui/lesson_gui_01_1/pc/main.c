// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_01_2 メッセージボックスを表示する */

#include <stdio.h>
#include <Windows.h>
#include "win_msgbox.h"

int main(void)
{
	message_box("計算結果", "10 + 20 = 30");

}
