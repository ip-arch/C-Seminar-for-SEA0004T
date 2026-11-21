// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_01 メッセージボックスを表示する */

#include <stdio.h>
#include <Windows.h>

int main(void)
{
    MessageBoxW(
        NULL,
        L"Hello, Windows!",
        L"Message",
        MB_OK
    );
}
