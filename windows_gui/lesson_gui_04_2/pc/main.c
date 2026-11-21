// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* センサー選択サンプル (PC 側)
 *   プルダウンで選んだセンサーを、ボードに問い合わせて、メッセージボックスに表示する。
 *   センサーの一覧は、protocol.h の SENSOR_LIST から作る。 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart_helper.h"
#include "../protocol.h"
#include "gui_app.h"
#include "win_msgbox.h"

typedef struct {
    const char* token;      /* 通信での名前 */
    const char* name;       /* プルダウンに出す名前 */
    int         nvalues;    /* 値の個数 */
    int         decimals;   /* 小数点以下の桁数 */
    const char* unit;       /* 単位 */
} SensorInfo;

/* 表は、1 つの一覧から作る。並びは、protocol.h の enum と同じ */
static const SensorInfo kSensors[SENSOR_COUNT] = {
#define X(id, tok, ja, n, dec, unit)  { tok, ja, n, dec, unit },
    SENSOR_LIST(X)
#undef X
};

/* プルダウンに渡す名前の一覧 */
static const char* kNames[SENSOR_COUNT] = {
#define X(id, tok, ja, n, dec, unit)  ja,
    SENSOR_LIST(X)
#undef X
};

static UartStream g_uart;   /* ボタンの処理から使うので、関数の外に置く */

/* 整数 v を、小数点以下 dec 桁の文字列にする。例: (979, 2) → "9.79" */
static void format_fixed(char* buf, size_t size, long v, int dec)
{
    if (dec <= 0) {
        snprintf(buf, size, "%ld", v);
        return;
    }
    long scale = 1;
    for (int i = 0; i < dec; i++) scale *= 10;
    long a = (v < 0) ? -v : v;
    snprintf(buf, size, "%s%ld.%0*ld", (v < 0) ? "-" : "", a / scale, dec, a % scale);
}

/* 応答 "ACCEL 12 -5 979" から、名前を確かめて、整数を n 個取り出す。成功したら 1 */
static int parse_response(const char* line, const char* token, int n, long v[])
{
    size_t len = strlen(token);
    if (strncmp(line, token, len) != 0 || line[len] != ' ') {
        return 0;                                   /* 想定外の応答 (ERROR を含む) */
    }
    const char* p = line + len;
    for (int i = 0; i < n; i++) {
        char* end;
        v[i] = strtol(p, &end, 10);
        if (end == p) return 0;                     /* 数が足りない */
        p = end;
    }
    return 1;
}

/* ボードに聞いて、値を受け取る。成功したら 1 */
static int query_sensor(const SensorInfo* s, long v[])
{
    char line[96];

    fprintf(g_uart.out, CMD_GET " %s\n", s->token);          /* 例: GET TEMP */
    if (uart_fgets(line, sizeof line, &g_uart) == NULL) {
        return 0;                                            /* 時間切れ */
    }
    return parse_response(line, s->token, s->nvalues, v);
}

void on_button_click(void)
{
    int idx = get_select_index();
    long v[SENSOR_MAX_VALUES];
    char text[96] = "";
    char msg[160];

    if (idx < 0 || idx >= SENSOR_COUNT) {
        return;
    }
    const SensorInfo* s = &kSensors[idx];       /* 選んだ番号で、表を引く */

    if (!query_sensor(s, v)) {
        snprintf(msg, sizeof msg, "%s の値を読めませんでした", s->name);
        message_box("センサー読み込み", msg);
        return;
    }

    for (int i = 0; i < s->nvalues; i++) {
        char one[24];
        size_t len = strlen(text);
        format_fixed(one, sizeof one, v[i], s->decimals);
        snprintf(text + len, sizeof text - len, "%s%s", (i > 0) ? "  " : "", one);
    }
    snprintf(msg, sizeof msg, "%s\n%s %s", s->name, text, s->unit);
    message_box("センサー読み込み", msg);
}

int main(void)
{
    console_utf8();

    if (!open_uart_stream_sea0004t(&g_uart, 115200)) {
        printf("エラー: ボードを開けません (%s)\n", uart_last_error());
        return 1;
    }
    if (!uart_wait_line(&g_uart, PROTO_READY, 5000)) {
        printf("エラー: ボードから %s が届きません\n", PROTO_READY);
        close_uart_stream(&g_uart);
        return 1;
    }

    run_gui_app(GUI_APP_SELECT_INPUT,
        "センサー読み取り",
        "読み取るセンサーを選んでください:",
        "読み込み",
        kNames, SENSOR_COUNT);

    close_uart_stream(&g_uart);
    return 0;
}