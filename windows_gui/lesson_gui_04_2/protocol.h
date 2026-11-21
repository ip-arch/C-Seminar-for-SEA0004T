// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>

/* protocol.h -- PC とボードで共通の「約束」
 *
 *   やりとりは、改行で終わる 1 行のテキスト。
 *     PC → ボード   GET <名前>             例: GET TEMP
 *     ボード → PC   <名前> <整数> ...      例: TEMP 243 / ACCEL 12 -5 979
 *                    ERROR                  (知らない名前、または、読めなかった)
 *
 *   センサーは、下の SENSOR_LIST に 1 行ずつ書く。
 *   ここに 1 行足すだけで、PC の「プルダウン」と、ボードの「名前の表」に反映される。
 *   (ボードの read_sensor() に、読み取りの処理を足すのは、別に必要)
 *
 *   整数だけを送る。小数は、「10 倍した整数」などにして送り、
 *   PC 側が、桁数(decimals)を見て、小数点を入れる。 (レッスン11、19)
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PROTO_READY  "READY"      /* ボードが起動したときに、1 行送る */
#define CMD_GET      "GET"        /* PC → ボード : GET <名前> */
#define RES_ERROR    "ERROR"      /* ボード → PC : 知らない名前、または、読めなかった */

#define SENSOR_MAX_VALUES  3      /* 1 回の応答に入る、整数の最大の個数 */

 /* ---- センサー一覧 (ここだけに書く) ------------------------------------
  *        ID     通信での名前  PC の表示名         値の個数 桁数 単位
  *  桁数 : 送られてくる整数を、何分の 1 にして表示するか (1 → 10 倍の値, 2 → 100 倍の値) */
#define SENSOR_LIST(X) \
    X(TEMP,  "TEMP",  "温度センサー",   1, 1, "℃")      \
    X(LUX,   "LUX",   "照度センサー",   1, 0, "lx")     \
    X(ACCEL, "ACCEL", "加速度センサー", 3, 2, "m/s^2")  \
    X(POT,   "POT",   "可変抵抗",       1, 0, "")       \
    X(SOUND, "SOUND", "音センサー",     1, 0, "")

  /* ID を、0, 1, 2, ... の番号にする。プルダウンの選択番号と、同じ並びになる */
enum {
#define X(id, tok, ja, n, dec, unit)  SENSOR_##id,
    SENSOR_LIST(X)
#undef X
    SENSOR_COUNT
};

#endif