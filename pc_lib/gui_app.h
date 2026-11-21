#ifndef GUI_APP_H
#define GUI_APP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
#define GUI_APP_TEXT_INPUT 1

    /* ウィンドウ・入力欄・ボタンを表示し、イベントループを開始する */
    void run_gui_app(
        int flag,
        const char* window_title,
        const char* prompt_label,
        const char* button_label);

    /* ウィンドウ内の入力欄に入力されている文字列（UTF-8）を取得する */
    void get_input_text(char* buf, size_t buf_size);

    /* 任意で入力欄の文字列をセットする */
    void set_input_text(const char* text);

    /* 任意でタイマー（ミリ秒間隔）を開始する（0 で停止） */
    void set_gui_timer(unsigned int interval_ms);

    /* 受講生が実装するイベントハンドラ（ボタン押下時） */
    void on_button_click(void);

    /* 任意で使えるイベントハンドラ（タイマー通知時） */
    void on_timer(void);

#ifdef __cplusplus
}
#endif

#endif /* GUI_APP_H */