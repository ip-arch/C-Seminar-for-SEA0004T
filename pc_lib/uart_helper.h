// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
﻿/* uart_helper.h -- PC (Windows) 側で、USB シリアル機器と通信するための共用ライブラリ
 *
 *   使い方:
 *     UartStream uart;
 *     open_uart_stream_sea0004t(&uart, 115200);    // SEA0004T を自動で探して開く
 *     fprintf(uart.out, "LED ON\n");               // 送る (fflush は不要)
 *     uart_fgets(line, sizeof line, &uart);        // 1 行受け取る
 *     close_uart_stream(&uart);
 *
 *   読み取り用 (in) と書き込み用 (out) は、別々の FILE* になっている。 */
#ifndef UART_HELPER_H
#define UART_HELPER_H

#include <stdio.h>

/* SEA0004T の USB シリアル (CH340) の VID / PID。
 * 違っていたら、デバイスマネージャーの「詳細」→「ハードウェア ID」で確認して直す */
#define SEA0004T_VID "1A86"
#define SEA0004T_PID "7523"

typedef struct {
    FILE *in;      /* 受信用 (fgets, fscanf などで読む) */
    FILE *out;     /* 送信用 (fprintf, fputs などで書く。バッファなし) */
} UartStream;

/* 開く。成功なら 1、失敗なら 0 (理由は uart_last_error() で分かる) */
int  open_uart_stream_sea0004t(UartStream *u, unsigned int baud);
int  open_uart_stream_by_vidpid(UartStream *u, const char *vid, const char *pid, unsigned int baud);
int  open_uart_stream_by_port(UartStream *u, const char *com_name, unsigned int baud);   /* 例: "COM5" */

/* 閉じる (COM ポートも解放される) */
void close_uart_stream(UartStream *u);

/* fgets と同じ。ただし、受信待ちが時間切れになっても、次の読み出しが続けられる */
char *uart_fgets(char *buf, int size, UartStream *u);

/* 受信待ちの時間切れ (ミリ秒)。既定は 1000 */
void uart_set_read_timeout(UartStream *u, unsigned int timeout_ms);

/* expected と一致する 1 行 (例: "READY") が届くまで待つ。届けば 1、時間切れなら 0。
 * ポートを開くとボードが再起動するので、最初に呼んで、起動の完了を待つ */
int  uart_wait_line(UartStream *u, const char *expected, unsigned int timeout_ms);

/* 直前の失敗の理由 */
const char *uart_last_error(void);

/* ミリ秒待つ */
void uart_sleep_ms(unsigned int ms);

/* コンソールの出力を UTF-8 にする (日本語が文字化けしないように) */
void console_utf8(void);

#endif
