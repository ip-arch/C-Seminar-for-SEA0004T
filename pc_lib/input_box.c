// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>


#include "input_box.h"

#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#define IDC_INPUT_LABEL 1001
#define IDC_INPUT_EDIT  1002
#define IDC_INPUT_OK    IDOK
#define IDC_INPUT_CANCEL IDCANCEL

typedef struct {
    const wchar_t* prompt_w;
    wchar_t* out_w;
    size_t out_w_cap;
    BOOL submitted;
    BOOL finished;
    HWND hwnd_edit;
    HFONT hfont;
} input_box_state_t;

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

static void apply_font_recursive(HWND hwnd, HFONT hfont)
{
    HWND child;
    if (hwnd == NULL || hfont == NULL) {
        return;
    }
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);
    child = GetWindow(hwnd, GW_CHILD);
    while (child != NULL) {
        SendMessageW(child, WM_SETFONT, (WPARAM)hfont, TRUE);
        child = GetWindow(child, GW_HWNDNEXT);
    }
}

static void finish_input_box(HWND hwnd, input_box_state_t* st, BOOL submitted)
{
    if (st == NULL || st->finished) {
        return;
    }
    st->submitted = submitted;
    if (submitted && st->hwnd_edit != NULL && st->out_w != NULL && st->out_w_cap > 0) {
        GetWindowTextW(st->hwnd_edit, st->out_w, (int)st->out_w_cap);
    }
    else if (st->out_w != NULL && st->out_w_cap > 0) {
        st->out_w[0] = L'\0';
    }
    st->finished = TRUE;
    if (hwnd != NULL) {
        DestroyWindow(hwnd);
    }
}

static LRESULT CALLBACK input_box_wnd_proc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    input_box_state_t* st = (input_box_state_t*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        st = (input_box_state_t*)cs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)st);

        st->hfont = CreateFontW(
            -16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Yu Gothic UI");

        CreateWindowExW(
            0, L"STATIC",
            (st && st->prompt_w) ? st->prompt_w : L"",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            16, 14, 340, 24,
            hwnd, (HMENU)(INT_PTR)IDC_INPUT_LABEL, cs->hInstance, NULL);

        st->hwnd_edit = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
            16, 44, 340, 26,
            hwnd, (HMENU)(INT_PTR)IDC_INPUT_EDIT, cs->hInstance, NULL);

        if (st && st->hwnd_edit && st->out_w_cap > 1) {
            SendMessageW(st->hwnd_edit, EM_SETLIMITTEXT, (WPARAM)(st->out_w_cap - 1), 0);
        }

        CreateWindowExW(
            0, L"BUTTON", L"OK",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            186, 84, 80, 28,
            hwnd, (HMENU)(INT_PTR)IDC_INPUT_OK, cs->hInstance, NULL);

        CreateWindowExW(
            0, L"BUTTON", L"キャンセル",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            276, 84, 80, 28,
            hwnd, (HMENU)(INT_PTR)IDC_INPUT_CANCEL, cs->hInstance, NULL);

        apply_font_recursive(hwnd, st ? st->hfont : NULL);
        if (st && st->hwnd_edit) {
            SetFocus(st->hwnd_edit);
        }
        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_INPUT_OK:
            finish_input_box(hwnd, st, TRUE);
            return 0;
        case IDC_INPUT_CANCEL:
            finish_input_box(hwnd, st, FALSE);
            return 0;
        }
        break;

    case WM_CLOSE:
        finish_input_box(hwnd, st, FALSE);
        return 0;

    case WM_DESTROY:
        if (st && st->hfont != NULL) {
            DeleteObject(st->hfont);
            st->hfont = NULL;
        }
        if (st) {
            st->finished = TRUE;
        }
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void input_box(const char* prompt, char* buf, size_t buf_size)
{
    static const wchar_t* kClassName = L"SeaInputBoxWindowClass";
    static BOOL class_registered = FALSE;

    HINSTANCE hinst;
    wchar_t* prompt_w;
    wchar_t* out_w;
    size_t out_w_cap;
    input_box_state_t st;
    int screen_w, screen_h, win_w, win_h, pos_x, pos_y;
    HWND hwnd;
    MSG msg;

    if (buf == NULL || buf_size == 0) {
        return;
    }
    buf[0] = '\0';

    hinst = GetModuleHandleW(NULL);
    if (!class_registered) {
        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = input_box_wnd_proc;
        wc.hInstance = hinst;
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = kClassName;
        if (RegisterClassExW(&wc) != 0) {
            class_registered = TRUE;
        }
    }

    prompt_w = utf8_to_wide_alloc(prompt);
    out_w_cap = (buf_size > 1) ? buf_size : 2;
    out_w = (wchar_t*)calloc(out_w_cap, sizeof(wchar_t));
    if (out_w == NULL) {
        free(prompt_w);
        return;
    }

    memset(&st, 0, sizeof(st));
    st.prompt_w = prompt_w;
    st.out_w = out_w;
    st.out_w_cap = out_w_cap;
    st.submitted = FALSE;
    st.finished = FALSE;

    win_w = 388;
    win_h = 160;
    screen_w = GetSystemMetrics(SM_CXSCREEN);
    screen_h = GetSystemMetrics(SM_CYSCREEN);
    pos_x = (screen_w - win_w) / 2;
    pos_y = (screen_h - win_h) / 2;
    if (pos_x < 0) pos_x = 0;
    if (pos_y < 0) pos_y = 0;

    hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        kClassName,
        L"入力",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        pos_x, pos_y, win_w, win_h,
        NULL, NULL, hinst, &st);

    if (hwnd != NULL) {
        while (!st.finished && GetMessageW(&msg, NULL, 0, 0) > 0) {
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
                finish_input_box(hwnd, &st, TRUE);
                continue;
            }
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                finish_input_box(hwnd, &st, FALSE);
                continue;
            }
            if (!IsDialogMessageW(hwnd, &msg)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    if (st.submitted) {
        wide_to_utf8_copy(out_w, buf, buf_size);
    }
    else {
        buf[0] = '\0';
    }

    free(out_w);
    free(prompt_w);
}