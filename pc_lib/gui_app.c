// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>


#include "gui_app.h"

#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#define IDC_MAIN_LABEL  2001
#define IDC_MAIN_EDIT   2002
#define IDC_MAIN_BUTTON 2003
#define IDC_MAIN_COMBO  2004
#define IDT_MAIN_TIMER  3001

static HWND g_hwnd_main = NULL;
static HWND g_hwnd_edit = NULL;
static HWND g_hwnd_combo = NULL;
static HFONT g_hfont_main = NULL;
static UINT_PTR g_timer_id = 0;

#if defined(_MSC_VER)
#pragma comment(linker, "/ALTERNATENAME:on_timer=default_on_timer")
void default_on_timer(void) {}
#else
__attribute__((weak)) void on_timer(void) {}
#endif

static wchar_t* utf8_to_wide_alloc(const char* utf8_str)
{
    int len;
    wchar_t* wstr;

    if (utf8_str == NULL) {
        utf8_str = "";
    }

    len = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
    if (len <= 0) {
        return NULL;
    }

    wstr = (wchar_t*)malloc((size_t)len * sizeof(wchar_t));
    if (wstr == NULL) {
        return NULL;
    }

    if (MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, wstr, len) <= 0) {
        free(wstr);
        return NULL;
    }

    return wstr;
}

static void wide_to_utf8_copy(const wchar_t* wstr, char* buf, size_t buf_size)
{
    int needed;

    if (buf == NULL || buf_size == 0) {
        return;
    }
    buf[0] = '\0';

    if (wstr == NULL) {
        return;
    }

    needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (needed <= 0) {
        return;
    }

    if ((size_t)needed <= buf_size) {
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, buf, (int)buf_size, NULL, NULL);
    }
    else {
        char* tmp = (char*)malloc((size_t)needed);
        if (tmp != NULL) {
            if (WideCharToMultiByte(CP_UTF8, 0, wstr, -1, tmp, needed, NULL, NULL) > 0) {
                memcpy(buf, tmp, buf_size - 1);
                buf[buf_size - 1] = '\0';
            }
            free(tmp);
        }
    }
}

void get_input_text(char* buf, size_t buf_size)
{
    int len_w;
    wchar_t* wbuf;

    if (buf == NULL || buf_size == 0) {
        return;
    }
    buf[0] = '\0';

    if (g_hwnd_edit == NULL) {
        return;
    }

    len_w = GetWindowTextLengthW(g_hwnd_edit);
    if (len_w <= 0) {
        return;
    }

    wbuf = (wchar_t*)calloc((size_t)len_w + 1, sizeof(wchar_t));
    if (wbuf == NULL) {
        return;
    }

    GetWindowTextW(g_hwnd_edit, wbuf, len_w + 1);
    wide_to_utf8_copy(wbuf, buf, buf_size);
    free(wbuf);
}

void set_input_text(const char* text)
{
    wchar_t* wstr;
    if (g_hwnd_edit == NULL) {
        return;
    }
    wstr = utf8_to_wide_alloc(text ? text : "");
    SetWindowTextW(g_hwnd_edit, wstr ? wstr : L"");
    free(wstr);
}

void set_gui_timer(unsigned int interval_ms)
{
    if (g_hwnd_main == NULL) {
        return;
    }
    if (g_timer_id != 0) {
        KillTimer(g_hwnd_main, g_timer_id);
        g_timer_id = 0;
    }
    if (interval_ms > 0) {
        g_timer_id = SetTimer(g_hwnd_main, IDT_MAIN_TIMER, interval_ms, NULL);
    }
}

void set_select_items(const char* const* items, int count)
{
    int i;
    if (g_hwnd_combo == NULL || items == NULL || count <= 0) {
        return;
    }

    SendMessageW(g_hwnd_combo, CB_RESETCONTENT, 0, 0);

    for (i = 0; i < count; i++) {
        wchar_t* wstr = utf8_to_wide_alloc(items[i] ? items[i] : "");
        if (wstr != NULL) {
            SendMessageW(g_hwnd_combo, CB_ADDSTRING, 0, (LPARAM)wstr);
            free(wstr);
        }
    }

    SendMessageW(g_hwnd_combo, CB_SETCURSEL, 0, 0);
}

int get_select_index(void)
{
    LRESULT idx;
    if (g_hwnd_combo == NULL) {
        return -1;
    }
    idx = SendMessageW(g_hwnd_combo, CB_GETCURSEL, 0, 0);
    return (idx == CB_ERR) ? -1 : (int)idx;
}

void get_select_text(char* buf, size_t buf_size)
{
    LRESULT idx;
    LRESULT len_w;
    wchar_t* wbuf;

    if (buf == NULL || buf_size == 0) {
        return;
    }
    buf[0] = '\0';

    if (g_hwnd_combo == NULL) {
        return;
    }

    idx = SendMessageW(g_hwnd_combo, CB_GETCURSEL, 0, 0);
    if (idx == CB_ERR) {
        return;
    }

    len_w = SendMessageW(g_hwnd_combo, CB_GETLBTEXTLEN, (WPARAM)idx, 0);
    if (len_w == CB_ERR || len_w < 0) {
        return;
    }

    wbuf = (wchar_t*)calloc((size_t)len_w + 1, sizeof(wchar_t));
    if (wbuf == NULL) {
        return;
    }

    SendMessageW(g_hwnd_combo, CB_GETLBTEXT, (WPARAM)idx, (LPARAM)wbuf);
    wide_to_utf8_copy(wbuf, buf, buf_size);
    free(wbuf);
}

typedef struct {
    int flag;
    const wchar_t* label_w;
    const wchar_t* button_w;
    const char* const* items;
    int item_count;
} app_create_param_t;

static void populate_combo_items(HWND hcombo, const char* const* items, int count)
{
    int i;
    if (hcombo == NULL || items == NULL || count <= 0) {
        return;
    }

    SendMessageW(hcombo, CB_RESETCONTENT, 0, 0);

    for (i = 0; i < count; i++) {
        wchar_t* wstr = utf8_to_wide_alloc(items[i] ? items[i] : "");
        if (wstr != NULL) {
            SendMessageW(hcombo, CB_ADDSTRING, 0, (LPARAM)wstr);
            free(wstr);
        }
    }

    SendMessageW(hcombo, CB_SETCURSEL, 0, 0);
}

static LRESULT CALLBACK main_wnd_proc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        app_create_param_t* param = (app_create_param_t*)cs->lpCreateParams;
        const wchar_t* label_text = (param && param->label_w) ? param->label_w : L"文字列を入力してください:";
        const wchar_t* btn_text = (param && param->button_w) ? param->button_w : L"表示";
        int next_y = 44;
        HWND hlabel, hbtn;

        g_hwnd_main = hwnd;
        g_hfont_main = CreateFontW(
            -16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Yu Gothic UI");

        hlabel = CreateWindowExW(
            0, L"STATIC", label_text,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            20, 16, 340, 24,
            hwnd, (HMENU)(INT_PTR)IDC_MAIN_LABEL, cs->hInstance, NULL);

        if (param && (param->flag & GUI_APP_SELECT_INPUT)) {
            g_hwnd_combo = CreateWindowExW(
                0, L"COMBOBOX", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
                20, next_y, 340, 140,
                hwnd, (HMENU)(INT_PTR)IDC_MAIN_COMBO, cs->hInstance, NULL);
            populate_combo_items(g_hwnd_combo, param->items, param->item_count);
            next_y += 38;
        }

        if (param && (param->flag & GUI_APP_TEXT_INPUT))
        g_hwnd_edit = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
            20, 44, 340, 28,
            hwnd, (HMENU)(INT_PTR)IDC_MAIN_EDIT, cs->hInstance, NULL);

        hbtn = CreateWindowExW(
            0, L"BUTTON", btn_text,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            20, 86, 140, 34,
            hwnd, (HMENU)(INT_PTR)IDC_MAIN_BUTTON, cs->hInstance, NULL);

        if (g_hfont_main != NULL) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
            if (hlabel)     SendMessageW(hlabel, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
            if (g_hwnd_edit) SendMessageW(g_hwnd_edit, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
            if (hbtn)       SendMessageW(hbtn, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
        }

        if (g_hwnd_combo != NULL) {
            SetFocus(g_hwnd_combo);
        } else if (g_hwnd_edit != NULL) {
            SetFocus(g_hwnd_edit);
        }
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_MAIN_BUTTON && HIWORD(wParam) == BN_CLICKED) {
            on_button_click();
            return 0;
        }
        break;

    case WM_TIMER:
        if (wParam == IDT_MAIN_TIMER) {
            on_timer();
            return 0;
        }
        break;

    case WM_CLOSE:
        if (g_timer_id != 0) {
            KillTimer(hwnd, g_timer_id);
            g_timer_id = 0;
        }
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        if (g_hfont_main != NULL) {
            DeleteObject(g_hfont_main);
            g_hfont_main = NULL;
        }
        g_hwnd_edit = NULL;
        g_hwnd_main = NULL;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void run_gui_app(
    int flag,
    const char* window_title,
    const char* prompt_label,
    const char* button_label,
    const char* const* items,
    int item_count)
{
    static const wchar_t* kClassName = L"SeaEventInputWindowClass";
    static BOOL registered = FALSE;

    HINSTANCE hinst = GetModuleHandleW(NULL);
    wchar_t* title_w = utf8_to_wide_alloc(window_title ? window_title : "演習2");
    wchar_t* label_w = utf8_to_wide_alloc(prompt_label ? prompt_label : "文字列を入力してください:");
    wchar_t* button_w = utf8_to_wide_alloc(button_label ? button_label : "表示");
    app_create_param_t param;
    int win_w = 396;
    int win_h = 176;
    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);
    int pos_x = (screen_w - win_w) / 2;
    int pos_y = (screen_h - win_h) / 2;
    HWND hwnd;
    MSG msg;

    if (!registered) {
        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = main_wnd_proc;
        wc.hInstance = hinst;
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = kClassName;
        if (RegisterClassExW(&wc) != 0) {
            registered = TRUE;
        }
    }

    if (pos_x < 0) pos_x = 0;
    if (pos_y < 0) pos_y = 0;
    param.flag = flag;
    param.label_w = label_w;
    param.button_w = button_w;
    param.items = items;
    param.item_count = item_count;

    hwnd = CreateWindowExW(
        0,
        kClassName,
        title_w ? title_w : L"演習2",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        pos_x, pos_y, win_w, win_h,
        NULL, NULL, hinst, &param);

    if (hwnd != NULL) {
        while (GetMessageW(&msg, NULL, 0, 0) > 0) {
            if (!IsDialogMessageW(hwnd, &msg)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    free(button_w);
    free(label_w);
    free(title_w);
}