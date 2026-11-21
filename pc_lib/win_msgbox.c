#include <stdlib.h>
#include <Windows.h>

static wchar_t* utf8_to_wide_alloc(const char* utf8_str)
{
    int wide_len;
    wchar_t* wide_str;

    if (utf8_str == NULL) {
        return NULL;
    }

    /* 必要なワイド文字数（終端の L'\0' を含む）を取得 */
    wide_len = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8_str,
        -1,
        NULL,
        0
    );
    if (wide_len <= 0) {
        return NULL;
    }

    wide_str = (wchar_t*)malloc((size_t)wide_len * sizeof(wchar_t));
    if (wide_str == NULL) {
        return NULL;
    }

    /* UTF-8 から UTF-16 (wchar_t) へ変換 */
    if (MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8_str,
        -1,
        wide_str,
        wide_len) <= 0) {
        free(wide_str);
        return NULL;
    }

    return wide_str;
}

void message_box(const char* title, const char* text)
{
    wchar_t* w_title = utf8_to_wide_alloc(title ? title : "");
    wchar_t* w_text = utf8_to_wide_alloc(text ? text : "");

    MessageBoxW(
        NULL,
        w_text ? w_text : L"",
        w_title ? w_title : L"",
        MB_OK
    );

    free(w_title);
    free(w_text);
}