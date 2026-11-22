// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_04_3 センサーデータをグラフ化。 ボタンによりグラフ記述をON/OFFする */

#include <stdio.h>
#include "uart_helper.h"
#include "../protocol.h"
#include "gui_app.h"
#include "win_msgbox.h"

UartStream g_uart;

typedef struct {
    const char* token;      /* 通信での名前 */
    const char* name;       /* プルダウンに出す名前 */
    int         nvalues;    /* 値の個数 */
    int         decimals;   /* 小数点以下の桁数 */
    const char* unit;       /* 単位 */
    int min;                /* グラフ最小値 */
    int max;                /* グラフ最大値 */
} SensorInfo;

/* 表は、1 つの一覧から作る。並びは、protocol.h の enum と同じ */
static const SensorInfo kSensors[SENSOR_COUNT] = {
#define X(id, tok, ja, n, dec, unit, min, max)  { tok, ja, n, dec, unit, min, max },
    SENSOR_LIST(X)
#undef X
};

/* プルダウンに渡す名前の一覧 */
static const char* kNames[SENSOR_COUNT] = {
#define X(id, tok, ja, n, dec, unit, min, max)  ja,
    SENSOR_LIST(X)
#undef X
};


void on_button_click(void)
{
    static int graph = 0;
    if (!graph) {
        set_gui_timer(500);
        message_box("グラフ", "スタート");
        int idx = get_select_index();
        int gmin = kSensors[idx].min, gmax = kSensors[idx].max;
        for (int i = 0; i < kSensors[idx].decimals; i++) {
            gmin *= 10;
            gmax *= 10;
        }
        /* 選択したセンサーに合わせてレンジやグラフを切り替え */
        graph_set_decimals(kSensors[idx].decimals);
        graph_set_range(gmin, gmax);
        }
    else {
        set_gui_timer(0);
        message_box("グラフ", "ストップ");
        }
    graph = !graph;
    graph_clear();
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

void on_timer(void)
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

    graph_add_value(v[0]);
}

int main(void)
{
    console_utf8();                                  /* 日本語を表示するための設定 */

    if (!open_uart_stream_sea0004t(&g_uart, 115200)) {
        printf("エラー: ボードを開けません (%s)\n", uart_last_error());
        return 1;
    }

    /* ポートを開くとボードが再起動するので、READY が届くのを待つ */
    printf("ボードの起動を待っています...\n");
    if (!uart_wait_line(&g_uart, PROTO_READY, 5000)) {
        printf("エラー: ボードから %s が届きません\n", PROTO_READY);
        close_uart_stream(&g_uart);
        return 1;
    }
    printf("接続しました\n");
    run_gui_app(
        GUI_APP_SELECT_INPUT | GUI_APP_GRAPH,
        "センサーモニター",
        "表示するセンサーを選んでください:",
        "切替・クリア",
        kNames,
        (int)(sizeof(kNames) / sizeof(kNames[0])));

    close_uart_stream(&g_uart);
    return 0;
}
