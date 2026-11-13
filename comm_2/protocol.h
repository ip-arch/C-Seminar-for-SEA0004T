// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* protocol.h -- PC とボードで共通の「約束」
 *   やりとりは、改行で終わる 1 行のテキスト。
 *   PC が "GET xxx" と聞くと、ボードが "xxx 数値" と答える */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PROTO_READY    "READY"        /* ボードが起動したときに、1 行送る */

#define CMD_GET_POT    "GET POT"      /* 可変抵抗 (0〜1023) */
#define CMD_GET_SOUND  "GET SOUND"    /* 音センサ (0〜1023) */

#define RES_POT        "POT"          /* 応答の形式: "POT 355" */
#define RES_SOUND      "SOUND"
#define RES_ERROR      "ERROR"        /* 知らない命令だった */

#endif
