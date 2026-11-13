// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_ir.h -- 赤外線リモコン受信 (D2 = INT0)。NEC 方式
 *   ir_init() の前に time_init() を呼ぶこと
 *
 *   ir_read() が返すのは NEC のコマンド値。キットのリモコンでは下のとおり。
 *   (Elecrow のサンプルの dta[8] は、この値のビットの並びを逆にしたもの。
 *    例: '1' は 0x0C → 逆順で 0x30 = 48) */
#ifndef SEA_IR_H
#define SEA_IR_H
#include <stdint.h>

#define IR_KEY_0          0x16
#define IR_KEY_1          0x0C
#define IR_KEY_2          0x18
#define IR_KEY_3          0x5E
#define IR_KEY_4          0x08
#define IR_KEY_5          0x1C
#define IR_KEY_6          0x5A
#define IR_KEY_7          0x42
#define IR_KEY_8          0x52
#define IR_KEY_9          0x4A
#define IR_KEY_EQ         0x09
#define IR_KEY_VOL_UP     0x15
#define IR_KEY_VOL_DOWN   0x07
#define IR_KEY_NEXT       0x40
#define IR_KEY_PLAY_PAUSE 0x43

void    ir_init(void);
uint8_t ir_available(void);   /* 新しいキーが届いていれば 1 */
uint8_t ir_read(void);        /* キーのコマンド値を返し、受信済みの印を消す */
#endif
