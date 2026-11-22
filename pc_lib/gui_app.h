// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>

#ifndef GUI_APP_H
#define GUI_APP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_APP_TEXT_INPUT   (1 << 0)
#define GUI_APP_SELECT_INPUT (1 << 1)
#define GUI_APP_GRAPH        (1 << 2)

    void run_gui_app(
        int flag,
        const char* window_title,
        const char* prompt_label,
        const char* button_label,
        const char* const* items,
        int item_count);

    /* テキスト入力用 */
    void get_input_text(char* buf, size_t buf_size);
    void set_input_text(const char* text);

    /* プルダウン（コンボボックス）用 */
    void set_select_items(const char* const* items, int count);
    int  get_select_index(void);
    void get_select_text(char* buf, size_t buf_size);

    /* グラフ表示用（GUI_APP_GRAPH 指定時に有効） */
    void graph_add_value(int value);
    void graph_clear(void);
    void graph_set_range(int min_value, int max_value);
    void graph_set_decimals(int decimals);

    /* タイマー・イベント */
    void set_gui_timer(unsigned int interval_ms);
    void on_button_click(void);
    void on_timer(void);

#ifdef __cplusplus
}
#endif

#endif /* GUI_APP_H */