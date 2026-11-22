// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>

#include "gui_app.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <Windows.h>

#define IDC_MAIN_LABEL   2001
#define IDC_MAIN_EDIT    2002
#define IDC_MAIN_BUTTON  2003
#define IDC_MAIN_COMBO   2004
#define IDT_MAIN_TIMER   3001

#define GRAPH_MAX_POINTS 100

static HWND g_hwnd_main = NULL;
static HWND g_hwnd_edit = NULL;
static HWND g_hwnd_combo = NULL;
static HWND g_hwnd_button = NULL;
static HFONT g_hfont_main = NULL;
static UINT_PTR g_timer_id = 0;

static int g_active_flag = 0;
static int g_graph_top_y = 140;

static int g_graph_values[GRAPH_MAX_POINTS];
static int g_graph_count = 0;
static int g_graph_min = 0;
static int g_graph_max = 100;

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

static void invalidate_graph_area(BOOL erase)
{
    RECT rc;
    if (g_hwnd_main == NULL || !(g_active_flag & GUI_APP_GRAPH)) {
        return;
    }
    GetClientRect(g_hwnd_main, &rc);
    rc.top = g_graph_top_y;
    if (rc.bottom > rc.top) {
        InvalidateRect(g_hwnd_main, &rc, erase);
    }
}

void graph_add_value(int value)
{
    if (g_graph_count < GRAPH_MAX_POINTS) {
        g_graph_values[g_graph_count++] = value;
    }
    else {
        memmove(&g_graph_values[0], &g_graph_values[1], sizeof(int) * (GRAPH_MAX_POINTS - 1));
        g_graph_values[GRAPH_MAX_POINTS - 1] = value;
    }
    invalidate_graph_area(FALSE);
}

void graph_clear(void)
{
    g_graph_count = 0;
    invalidate_graph_area(TRUE);
}

void graph_set_range(int min_value, int max_value)
{
    if (max_value <= min_value) {
        max_value = min_value + 1;
    }
    g_graph_min = min_value;
    g_graph_max = max_value;
    invalidate_graph_area(FALSE);
}

static int g_graph_decimals = 0;
static wchar_t g_graph_unit_w[32] = L"";

void graph_set_decimals(int decimals)
{
    if (decimals < 0) decimals = 0;
    if (decimals > 6) decimals = 6;
    g_graph_decimals = decimals;
    invalidate_graph_area(FALSE);
}

void graph_set_unit(const char* unit)
{
    wchar_t* wstr;
    g_graph_unit_w[0] = L'\0';
    if (unit != NULL && unit[0] != '\0') {
        wstr = utf8_to_wide_alloc(unit);
        if (wstr != NULL) {
            wcsncpy_s(g_graph_unit_w, _countof(g_graph_unit_w), wstr, _TRUNCATE);
            free(wstr);
        }
    }
    invalidate_graph_area(FALSE);
}

static int graph_value_to_y(int val, int top, int bottom)
{
    int height = bottom - top;
    double span = (double)(g_graph_max - g_graph_min);
    double ratio;
    double y_f;

    if (span <= 0.0 || height <= 0) {
        return bottom;
    }
    ratio = ((double)val - (double)g_graph_min) / span;
    y_f = (double)top + (double)height * (1.0 - ratio);

    if (y_f < (double)top) y_f = (double)top;
    if (y_f > (double)bottom) y_f = (double)bottom;
    return (int)(y_f + 0.5);
}

static void format_scaled_value_w(int raw_val, int decimals, wchar_t* out, size_t out_cap)
{
    int scale = 1;
    int i;

    if (out == NULL || out_cap == 0) {
        return;
    }

    if (decimals <= 0) {
        swprintf_s(out, out_cap, L"%d", raw_val);
        return;
    }

    for (i = 0; i < decimals; i++) {
        scale *= 10;
    }

    {
        int is_neg = (raw_val < 0);
        unsigned int abs_v = is_neg ? (unsigned int)(-(raw_val + 1)) + 1u : (unsigned int)raw_val;
        unsigned int uscale = (unsigned int)scale;
        unsigned int whole = abs_v / uscale;
        unsigned int frac = abs_v % uscale;

        if (is_neg) {
            swprintf_s(out, out_cap, L"-%u.%0*u", whole, decimals, frac);
        }
        else {
            swprintf_s(out, out_cap, L"%u.%0*u", whole, decimals, frac);
        }
    }
}

static void draw_graph_region(HWND hwnd, HDC hdc)
{
    RECT client_rc;
    RECT graph_rc;
    int width, height, graph_h;
    HDC mem_dc;
    HBITMAP mem_bmp, old_bmp;
    HBRUSH bg_brush;
    HPEN grid_pen, graph_pen, old_pen;
    int i;

    if (!(g_active_flag & GUI_APP_GRAPH)) {
        return;
    }

    GetClientRect(hwnd, &client_rc);
    width = client_rc.right - client_rc.left;
    height = client_rc.bottom - client_rc.top;
    graph_rc.left = 16;
    graph_rc.right = width - 16;
    graph_rc.top = g_graph_top_y;
    graph_rc.bottom = height - 16;

    graph_h = graph_rc.bottom - graph_rc.top;
    if (graph_rc.right <= graph_rc.left || graph_h <= 10) {
        return;
    }

    mem_dc = CreateCompatibleDC(hdc);
    mem_bmp = CreateCompatibleBitmap(hdc, width, height);
    old_bmp = (HBITMAP)SelectObject(mem_dc, mem_bmp);

    /* 背景（上部のコントロール領域も合わせる） */
    bg_brush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(mem_dc, &client_rc, bg_brush);
    DeleteObject(bg_brush);

    /* グラフ描画領域の背景 */
    bg_brush = CreateSolidBrush(RGB(20, 20, 20));
    FillRect(mem_dc, &graph_rc, bg_brush);
    DeleteObject(bg_brush);

    /* 横グリッド線 */
    grid_pen = CreatePen(PS_DOT, 1, RGB(60, 60, 60));
    old_pen = (HPEN)SelectObject(mem_dc, grid_pen);
    for (i = 1; i < 4; i++) {
        int y = graph_rc.top + (graph_h * i) / 4;
        MoveToEx(mem_dc, graph_rc.left, y, NULL);
        LineTo(mem_dc, graph_rc.right, y);
    }

    /* 折れ線グラフ */
    if (g_graph_count > 1) {
        int plot_w = graph_rc.right - graph_rc.left;
        graph_pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 100));
        SelectObject(mem_dc, graph_pen);

        for (i = 0; i < g_graph_count - 1; i++) {
            int x1 = graph_rc.left + (i * plot_w) / (GRAPH_MAX_POINTS - 1);
            int y1 = graph_value_to_y(g_graph_values[i], graph_rc.top, graph_rc.bottom);
            int x2 = graph_rc.left + ((i + 1) * plot_w) / (GRAPH_MAX_POINTS - 1);
            int y2 = graph_value_to_y(g_graph_values[i + 1], graph_rc.top, graph_rc.bottom);

            MoveToEx(mem_dc, x1, y1, NULL);
            LineTo(mem_dc, x2, y2);
        }

        SelectObject(mem_dc, old_pen);
        DeleteObject(graph_pen);
    }
    else {
        SelectObject(mem_dc, old_pen);
    }

    DeleteObject(grid_pen);

    /* グラフ領域のみ画面へ転送（ボタンやコンボボックスの上には被せない） */
    BitBlt(
        hdc,
        graph_rc.left,
        graph_rc.top,
        graph_rc.right - graph_rc.left,
        graph_h,
        mem_dc,
        graph_rc.left,
        graph_rc.top,
        SRCCOPY);
    /* BitBlt でグラフ描画領域を画面へ転送した後にラベルを描画 */
    {
        wchar_t val_str[64];
        wchar_t line_buf[128];
        HFONT old_font = NULL;
        int old_bk_mode = SetBkMode(hdc, TRANSPARENT);
        COLORREF old_color = SetTextColor(hdc, RGB(220, 220, 220));

        if (g_hfont_main != NULL) {
            old_font = (HFONT)SelectObject(hdc, g_hfont_main);
        }

        /* 左上：最大値 */
        format_scaled_value_w(g_graph_max, g_graph_decimals, val_str, _countof(val_str));
        swprintf_s(line_buf, _countof(line_buf), L"MAX: %s%s", val_str, g_graph_unit_w);
        TextOutW(hdc, graph_rc.left + 8, graph_rc.top + 6, line_buf, (int)wcslen(line_buf));

        /* 左下：最小値 */
        format_scaled_value_w(g_graph_min, g_graph_decimals, val_str, _countof(val_str));
        swprintf_s(line_buf, _countof(line_buf), L"MIN: %s%s", val_str, g_graph_unit_w);
        TextOutW(hdc, graph_rc.left + 8, graph_rc.bottom - 24, line_buf, (int)wcslen(line_buf));

        /* 右上：最新値 */
        if (g_graph_count > 0) {
            int latest = g_graph_values[g_graph_count - 1];
            format_scaled_value_w(latest, g_graph_decimals, val_str, _countof(val_str));
            swprintf_s(line_buf, _countof(line_buf), L"現在: %s%s", val_str, g_graph_unit_w);
            SetTextColor(hdc, RGB(120, 255, 160));
            {
                SIZE sz;
                GetTextExtentPoint32W(hdc, line_buf, (int)wcslen(line_buf), &sz);
                TextOutW(hdc, graph_rc.right - sz.cx - 8, graph_rc.top + 6, line_buf, (int)wcslen(line_buf));
            }
        }

        if (old_font != NULL) {
            SelectObject(hdc, old_font);
        }
        SetTextColor(hdc, old_color);
        SetBkMode(hdc, old_bk_mode);
    }
    SelectObject(mem_dc, old_bmp);
    DeleteObject(mem_bmp);
    DeleteDC(mem_dc);
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
        HWND hlabel;

        g_hwnd_main = hwnd;
        g_active_flag = param ? param->flag : 0;

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

        if (param && (param->flag & GUI_APP_TEXT_INPUT)) {
            g_hwnd_edit = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                20, next_y, 340, 28,
                hwnd, (HMENU)(INT_PTR)IDC_MAIN_EDIT, cs->hInstance, NULL);
            next_y += 40;
        }

        g_hwnd_button = CreateWindowExW(
            0, L"BUTTON", btn_text,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            20, next_y + 4, 140, 34,
            hwnd, (HMENU)(INT_PTR)IDC_MAIN_BUTTON, cs->hInstance, NULL);

        g_graph_top_y = next_y + 50;

        if (g_hfont_main != NULL) {
            SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
            if (hlabel)       SendMessageW(hlabel, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
            if (g_hwnd_combo) SendMessageW(g_hwnd_combo, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
            if (g_hwnd_edit)  SendMessageW(g_hwnd_edit, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
            if (g_hwnd_button)SendMessageW(g_hwnd_button, WM_SETFONT, (WPARAM)g_hfont_main, TRUE);
        }

        if (g_hwnd_combo != NULL) {
            SetFocus(g_hwnd_combo);
        }
        else if (g_hwnd_edit != NULL) {
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

    case WM_PAINT:
        if (g_active_flag & GUI_APP_GRAPH) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            draw_graph_region(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        break;

    case WM_SIZE:
        if (g_active_flag & GUI_APP_GRAPH) {
            invalidate_graph_area(TRUE);
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
        g_hwnd_button = NULL;
        g_hwnd_combo = NULL;
        g_hwnd_edit = NULL;
        g_hwnd_main = NULL;
        g_active_flag = 0;
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
    wchar_t* title_w = utf8_to_wide_alloc(window_title ? window_title : "演習");
    wchar_t* label_w = utf8_to_wide_alloc(prompt_label ? prompt_label : "文字列を入力してください:");
    wchar_t* button_w = utf8_to_wide_alloc(button_label ? button_label : "表示");
    app_create_param_t param;
    int win_w = (flag & GUI_APP_GRAPH) ? 800 : 396;
    int win_h = (flag & GUI_APP_GRAPH) ? 520 : 176;
    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);
    int pos_x = (screen_w - win_w) / 2;
    int pos_y = (screen_h - win_h) / 2;
    HWND hwnd;
    MSG msg;

    g_graph_decimals = 0; 
    g_graph_unit_w[0] = L'\0';

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

    g_graph_count = 0;

    hwnd = CreateWindowExW(
        0,
        kClassName,
        title_w ? title_w : L"演習",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_VISIBLE,
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