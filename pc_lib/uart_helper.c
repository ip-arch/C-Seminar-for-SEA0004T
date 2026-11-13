// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "uart_helper.h"
#include <windows.h>
#include <setupapi.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
#pragma comment(lib, "setupapi.lib")
#endif

/* ポート (COM と LPT) のデバイスクラス GUID {4D36E978-E325-11CE-BFC1-08002BE10318} */
static const GUID PORTS_CLASS_GUID =
    { 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } };

static char last_error[128] = "";
static unsigned int default_timeout_ms = 1000;

static void set_error(const char *message)
{
    strncpy(last_error, message, sizeof last_error - 1);
    last_error[sizeof last_error - 1] = '\0';
}

const char *uart_last_error(void) { return last_error; }
void uart_sleep_ms(unsigned int ms) { Sleep(ms); }

void console_utf8(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

static void to_upper(char *s)
{
    for (; *s; s++) *s = (char)toupper((unsigned char)*s);
}

/* 指定した VID / PID を持つ COM ポートの名前 (例: "COM5") を探す */
static int find_com_port(const char *vid, const char *pid, char *port, DWORD port_size)
{
    char pattern[64];
    snprintf(pattern, sizeof pattern, "VID_%s&PID_%s", vid, pid);
    to_upper(pattern);

    HDEVINFO devs = SetupDiGetClassDevsA(&PORTS_CLASS_GUID, NULL, NULL, DIGCF_PRESENT);
    if (devs == INVALID_HANDLE_VALUE) {
        set_error("デバイスの一覧を取得できません");
        return 0;
    }

    int found = 0;
    SP_DEVINFO_DATA info;
    info.cbSize = sizeof info;
    for (DWORD i = 0; !found && SetupDiEnumDeviceInfo(devs, i, &info); i++) {
        char ids[1024];
        DWORD size = 0;
        memset(ids, 0, sizeof ids);
        if (!SetupDiGetDeviceRegistryPropertyA(devs, &info, SPDRP_HARDWAREID, NULL,
                                               (PBYTE)ids, sizeof ids - 2, &size)) {
            continue;
        }
        /* ハードウェア ID は、文字列が '\0' でつながった形 (最後は '\0' が 2 つ) */
        int match = 0;
        for (char *p = ids; *p; p += strlen(p) + 1) {
            char copy[256];
            strncpy(copy, p, sizeof copy - 1);
            copy[sizeof copy - 1] = '\0';
            to_upper(copy);
            if (strstr(copy, pattern) != NULL) { match = 1; break; }
        }
        if (!match) continue;

        HKEY key = SetupDiOpenDevRegKey(devs, &info, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (key != INVALID_HANDLE_VALUE) {
            DWORD type = 0, n = port_size;
            if (RegQueryValueExA(key, "PortName", NULL, &type, (LPBYTE)port, &n) == ERROR_SUCCESS) {
                found = 1;
            }
            RegCloseKey(key);
        }
    }
    SetupDiDestroyDeviceInfoList(devs);

    if (!found) set_error("該当する USB シリアル機器が見つかりません");
    return found;
}

static void apply_timeout(HANDLE h, unsigned int timeout_ms)
{
    /* 受信: 1 文字でも届けばすぐ返す。何も届かなければ timeout_ms まで待つ */
    COMMTIMEOUTS t;
    memset(&t, 0, sizeof t);
    t.ReadIntervalTimeout        = MAXDWORD;
    t.ReadTotalTimeoutMultiplier = MAXDWORD;
    t.ReadTotalTimeoutConstant   = timeout_ms;
    t.WriteTotalTimeoutConstant  = 1000;
    SetCommTimeouts(h, &t);
}

static int open_handle(const char *com_name, unsigned int baud, HANDLE *out)
{
    char path[64];
    snprintf(path, sizeof path, "\\\\.\\%s", com_name);     /* COM10 以上にも対応 */

    HANDLE h = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        set_error("ポートを開けません (ほかのプログラムが使用中の可能性があります)");
        return 0;
    }

    DCB dcb;
    memset(&dcb, 0, sizeof dcb);
    dcb.DCBlength = sizeof dcb;
    if (!GetCommState(h, &dcb)) {
        set_error("ポートの設定を読み出せません");
        CloseHandle(h);
        return 0;
    }
    dcb.BaudRate = baud;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary  = TRUE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX  = FALSE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    if (!SetCommState(h, &dcb)) {
        set_error("ポートの設定に失敗しました");
        CloseHandle(h);
        return 0;
    }

    apply_timeout(h, default_timeout_ms);
    *out = h;
    return 1;
}

/* 1 つのハンドルから、読み取り用と書き込み用の FILE* を作る */
static int make_streams(UartStream *u, HANDLE h)
{
    HANDLE h2 = NULL;
    if (!DuplicateHandle(GetCurrentProcess(), h, GetCurrentProcess(), &h2,
                         0, FALSE, DUPLICATE_SAME_ACCESS)) {
        set_error("ハンドルを複製できません");
        CloseHandle(h);
        return 0;
    }

    int fd_in = _open_osfhandle((intptr_t)h, _O_RDONLY | _O_TEXT);
    if (fd_in == -1) {
        set_error("入力ストリームを作れません");
        CloseHandle(h);
        CloseHandle(h2);
        return 0;
    }
    int fd_out = _open_osfhandle((intptr_t)h2, _O_WRONLY | _O_TEXT);
    if (fd_out == -1) {
        set_error("出力ストリームを作れません");
        _close(fd_in);
        CloseHandle(h2);
        return 0;
    }

    u->in  = _fdopen(fd_in,  "rt");
    u->out = _fdopen(fd_out, "wt");
    if (u->in == NULL || u->out == NULL) {
        set_error("FILE* を作れません");
        if (u->in)  fclose(u->in);  else _close(fd_in);
        if (u->out) fclose(u->out); else _close(fd_out);
        u->in = u->out = NULL;
        return 0;
    }
    setvbuf(u->out, NULL, _IONBF, 0);          /* 書いたらすぐ送る (fflush 不要) */
    return 1;
}

int open_uart_stream_by_port(UartStream *u, const char *com_name, unsigned int baud)
{
    HANDLE h;
    u->in = u->out = NULL;
    last_error[0] = '\0';
    if (!open_handle(com_name, baud, &h)) return 0;
    return make_streams(u, h);
}

int open_uart_stream_by_vidpid(UartStream *u, const char *vid, const char *pid, unsigned int baud)
{
    char port[32] = "";
    u->in = u->out = NULL;
    last_error[0] = '\0';
    if (!find_com_port(vid, pid, port, sizeof port)) return 0;
    return open_uart_stream_by_port(u, port, baud);
}

int open_uart_stream_sea0004t(UartStream *u, unsigned int baud)
{
    return open_uart_stream_by_vidpid(u, SEA0004T_VID, SEA0004T_PID, baud);
}

void close_uart_stream(UartStream *u)
{
    if (u == NULL) return;
    if (u->out) fclose(u->out);                /* 内部のハンドルも閉じられる */
    if (u->in)  fclose(u->in);
    u->in = u->out = NULL;
}

char *uart_fgets(char *buf, int size, UartStream *u)
{
    clearerr(u->in);                           /* 前回の時間切れを引きずらない */
    return fgets(buf, size, u->in);
}

void uart_set_read_timeout(UartStream *u, unsigned int timeout_ms)
{
    default_timeout_ms = timeout_ms;
    if (u != NULL && u->in != NULL) {
        apply_timeout((HANDLE)_get_osfhandle(_fileno(u->in)), timeout_ms);
    }
}

int uart_wait_line(UartStream *u, const char *expected, unsigned int timeout_ms)
{
    char line[128];
    ULONGLONG end = GetTickCount64() + timeout_ms;
    while (GetTickCount64() < end) {
        if (uart_fgets(line, sizeof line, u) != NULL) {
            line[strcspn(line, "\r\n")] = '\0';
            if (strcmp(line, expected) == 0) return 1;
        }
    }
    set_error("待っていた応答が届きませんでした");
    return 0;
}
