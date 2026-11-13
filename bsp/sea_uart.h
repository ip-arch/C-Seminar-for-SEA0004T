// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_uart.h -- シリアル通信 (115200bps)
 *
 *   uart_init() のあと、標準入出力が使える。
 *     printf / puts : PC へ送る
 *     fgets(buf, n, stdin) : PC から 1 行受け取る (改行が来るまで待つ)
 *   受信は割り込みで受けてバッファ (64 バイト) に溜めるので、
 *   他の処理をしている間に届いた文字も取りこぼさない。 */
#ifndef SEA_UART_H
#define SEA_UART_H
#include <stdint.h>

void    uart_init(void);
uint8_t uart_available(void);   /* 受け取り済みの文字数 (待たずに調べる) */
int     uart_getc(void);        /* 1 文字受け取る。届くまで待つ */
#endif
