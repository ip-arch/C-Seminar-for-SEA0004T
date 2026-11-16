// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_01_reg: LED を 1 秒ごとに点滅させる (レジスタ版) */

#include "sea0004t.h"
#include "sea_time.h"
int main(void)
{
    DDRB |= (1 << PB2);              /* PB2を出力に設定 */
    for (;;) {
        PORTB |= (1 << PB2);         /* LED点灯 */
        delay_ms(1000);
        PORTB &= ~(1 << PB2);        /* LED消灯 */
        delay_ms(1000);
    }
}
