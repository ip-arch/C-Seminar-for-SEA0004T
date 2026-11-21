// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* protocol.h -- PC とボードで共通の「約束」
 *   どちらのプログラムもこのファイルを読むので、命令の文字列が食い違わない。
 *   やりとりは、改行で終わる 1 行のテキスト。 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PROTO_READY  "READY"      /* ボードが起動したときに、1 行送る */

#define CMD_LED_ON   "LED ON"     /* PC → ボード : LED を点ける */
#define CMD_LED_OFF  "LED OFF"    /* PC → ボード : LED を消す */

#define RES_OK       "OK"         /* ボード → PC : 実行した */
#define RES_ERROR    "ERROR"      /* ボード → PC : 知らない命令だった */

#endif
